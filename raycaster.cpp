#include "raycaster.hpp"
#include <math.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include "globals.hpp"

#define PI 3.141592

RayCaster::RayCaster(const Point3D& eye, const Background& bg, const SceneNode *root, const list<Light *> &lights, const Colour &ambient)
    : eye(eye), bg(bg), root(root), lights(lights), ambient(ambient), collider(root) {

    maxRecursionDepth = maxRayRecursionDepth;
}

cast_result RayCaster::cast(const Point3D &pos, const Vector3D &dir) const {
    list<collision_result> hits = collider.getCollisionData(pos, dir);

    cast_result castResult;
    castResult.hit = (hits.size() > 0) ? true : false;

    if (!castResult.hit) {
        return castResult;
    }

    // Find the closest hit

    collision_result closestHit;
    double minDistSq = INFINITY;

    for (list<collision_result>::iterator it = hits.begin(); it != hits.end(); it++) {
        double distSq = pos.distSq((*it).point);

        // We've collided with something too close, we were probably
        // casting from the surface and rehit the same object, so ignore
        if (distSq < EPSILON) {
            continue;
        }

        if (distSq < minDistSq) {
            minDistSq = distSq;
            closestHit = (*it);
        }
    }

    castResult.collisionResult = closestHit;
    if (minDistSq == INFINITY) {
        castResult.hit = false;
        return castResult;
    }

    castResult.collisionResult = closestHit;

    return castResult;
}

extern bool debug;

cast_result RayCaster::colourCast(const Point3D &pos, const Vector3D &dir) const {
    cast_result result = recursiveColourCast(pos, dir, 0);

    if (debug) {
        debug = false;
    }
    return result;
}

// returns a random number between 0 and 1
double RayCaster::getRandom() const {
    return ((double)rand())/(double)RAND_MAX;
}

Vector3D RayCaster::perturbVector(const Vector3D &dir, double circleRadius) const {
    if (circleRadius == 0) {
        return dir;
    }

    Vector3D crossAxis;

    if (dir[0] < dir[1] && dir[0] < dir[2]) {
        crossAxis = Vector3D(1, 0, 0);
    } else if (dir[1] < dir[2]) {
        crossAxis = Vector3D(0, 1, 0);
    } else {
        crossAxis = Vector3D(0, 0, 1);
    }

    Vector3D basis1 = dir.cross(crossAxis);
    Vector3D basis2 = dir.cross(basis1);

    double u;
    double v;

    double uSquared;
    double vSquared;

    do {
        u = getRandom() * (2 * circleRadius) - circleRadius;
        v = getRandom() * (2 * circleRadius) - circleRadius;
        uSquared = u * u;
        vSquared = v * v;
    } while ((uSquared + vSquared) >= (circleRadius * circleRadius));

    Vector3D perturbedVector = (u * basis1) + (v * basis2) + ((1 - uSquared - vSquared) * dir);
    perturbedVector.normalize();

    return perturbedVector;
}

cast_result RayCaster::recursiveColourCast(const Point3D &pos, const Vector3D &dir, int recursionDepth) const {
    cast_result primaryCast = cast(pos, dir);

    if (!primaryCast.hit) {
        return primaryCast;
    }

    const PhongMaterial *surfaceMaterial = primaryCast.collisionResult.surfaceMaterial;

    bool isReflective = surfaceMaterial->is_reflective();
    double glossiness = surfaceMaterial->get_glossiness();
    double reflectivity = surfaceMaterial->get_reflectivity();

    Colour finalColour(0);

    for (list<Light *>::const_iterator it = lights.begin(); it != lights.end(); it++) {
        Colour lightColour = shadeFromLight(primaryCast, (*it));
        finalColour = finalColour + lightColour;
    }

    if (recursionDepth < maxRecursionDepth && isReflective) {
        Point3D collisionPoint = primaryCast.collisionResult.point;
        Vector3D collisionNormal = primaryCast.collisionResult.normal;

        Vector3D reflectionDirection = (-2 * (dir.dot(collisionNormal)) * collisionNormal) + dir;
        reflectionDirection.normalize();

        Colour reflectionColour(0);

        int numHits = 0;

        for (int i = 0; i < numDistributedRays; i++) {
            Vector3D stochasticDirection = perturbVector(reflectionDirection, glossiness);
            cast_result recursiveCast = recursiveColourCast(collisionPoint, stochasticDirection, recursionDepth + 1);

            if (recursiveCast.hit) {
                reflectionColour = reflectionColour + recursiveCast.finalColour;
                numHits++;
            }
        }

        if (numHits > 0) {
            reflectionColour = (1.0 / numHits) * reflectionColour;
            finalColour = ((1 - reflectivity) * finalColour) + (reflectivity * reflectionColour);
        }

    }

    primaryCast.finalColour = finalColour + ambient * surfaceMaterial->get_diffuse();

    return primaryCast;
}

Colour RayCaster::calculateColourFromPointLight(struct cast_result primaryCast, const Light *light) const {
    Colour colour(0);

    cast_result castResult;
    Point3D position = primaryCast.collisionResult.point;

    Vector3D shadowCastDirection = light->position - position;
    shadowCastDirection.normalize();

    castResult = cast(position, shadowCastDirection);

    if (!castResult.hit || light->position.dist(position) < castResult.collisionResult.hitDistance) {
        double distSq = position.distSq(light->position);
        Vector3D lightVec = light->position - position;
        lightVec.normalize();
        Vector3D normal = primaryCast.collisionResult.normal;

        double lightDotNormal = max(lightVec.dot(normal), 0.0);
        const double *falloff = light->falloff;
        double energyIn = lightDotNormal;
        energyIn /= (falloff[0] + falloff[1] * sqrt(distSq) + falloff[2] * distSq);

        Vector3D r = (-1 * lightVec) + (2 * lightDotNormal * normal);
        r.normalize();

        Vector3D eyeVec = eye - position;
        eyeVec.normalize();

        double rDotEye = max(r.dot(eyeVec), 0.0);

        const PhongMaterial *surfaceMaterial = primaryCast.collisionResult.surfaceMaterial;
        Colour materialPropertiesColour = surfaceMaterial->get_diffuse();

        if (lightDotNormal > 0.0) {
            materialPropertiesColour = materialPropertiesColour + (pow(rDotEye, surfaceMaterial->get_shininess())) / normal.dot(lightVec) * surfaceMaterial->get_spec();
            colour = light->colour * materialPropertiesColour * energyIn;
        }
    }

    return colour;
}

Colour RayCaster::sampleColourFromAreaLight(struct cast_result primaryCast, const Light *light, int cellX, int cellY) const {
    Colour colour(0);

    cast_result castResult;
    Point3D position = primaryCast.collisionResult.point;

    double monteCarloOffsetX = getRandom() * - 0.5;
    double monteCarloOffsetY = getRandom() * - 0.5;

    // monteCarloOffsetX = 0;
    // monteCarloOffsetY = 0;

    Vector3D rightVecOffset = ((1.0 / areaLightSampleDimension) * ((double)cellX + 0.5 + monteCarloOffsetX)) * light->rightVec;
    Vector3D downVecOffset = ((1.0 / areaLightSampleDimension) * ((double)cellY + 0.5 + monteCarloOffsetY)) * light->downVec;

    Point3D samplePosition = light->position;
    samplePosition = samplePosition + rightVecOffset + downVecOffset;

    Vector3D shadowCastDirection = samplePosition - position;
    shadowCastDirection.normalize();

    castResult = cast(position, shadowCastDirection);

    // if (debug) {
    //     cerr << "LIGHT POSITION " << samplePosition << endl;
    //     cerr << "LIGHT DISTANCE " << samplePosition.dist(position) << endl;
    //     cerr << "START POSITION " << position << endl;
    //     cerr << "SHADOW CAST DIRECTION " << shadowCastDirection << endl;
    // }

    // if (debug) {
    //     if (castResult.hit) {
    //         cerr << "HIT POSITION " << castResult.collisionResult.point << endl;
    //         cerr << "HIT DISTANCE " << castResult.collisionResult.hitDistance << endl;
    //         cerr << "HIT NORMAL " << castResult.collisionResult.normal << endl;
    //     } else {
    //         cerr << "MISS!" << endl;
    //     }
    // }

    if (!castResult.hit || samplePosition.dist(position) < castResult.collisionResult.hitDistance) {
        double distSq = position.distSq(samplePosition);
        Vector3D lightVec = samplePosition - position;
        lightVec.normalize();
        Vector3D normal = primaryCast.collisionResult.normal;

        double lightDotNormal = max(lightVec.dot(normal), 0.0);
        const double *falloff = light->falloff;
        double energyIn = lightDotNormal;
        energyIn /= (falloff[0] + falloff[1] * sqrt(distSq) + falloff[2] * distSq);

        Vector3D r = (-1 * lightVec) + (2 * lightDotNormal * normal);
        r.normalize();

        Vector3D eyeVec = eye - position;
        eyeVec.normalize();

        double rDotEye = max(r.dot(eyeVec), 0.0);

        const PhongMaterial *surfaceMaterial = primaryCast.collisionResult.surfaceMaterial;
        Colour materialPropertiesColour = surfaceMaterial->get_diffuse();

        if (lightDotNormal > 0.0) {
            materialPropertiesColour = materialPropertiesColour + (pow(rDotEye, surfaceMaterial->get_shininess())) / normal.dot(lightVec) * surfaceMaterial->get_spec();
            colour = light->colour * materialPropertiesColour * energyIn;
        }
    }

    // if (debug) {
    //     cerr << "COLOUR " << colour << endl;
    //     cerr << "==================================================" << endl;
    // }

    return colour;
}

Colour RayCaster::shadeFromLight(struct cast_result primaryCast, const Light *light) const {
    // if (debug) {
    //     cerr << "LIGHT POSITION " << light->position << endl;
    //     cerr << "LIGHT DISTANCE " << light->position.dist(position) << endl;
    //     cerr << "START POSITION " << primaryCast.collisionResult.point << endl;
    //     cerr << "SHADOW CAST DIRECTION " << shadowCastDirection << endl;
    // }

    // if (debug) {
    //     if (castResult.hit) {
    //         cerr << "HIT POSITION " << castResult.collisionResult.point << endl;
    //         cerr << "HIT DISTANCE " << castResult.collisionResult.hitDistance << endl;
    //         cerr << "HIT NORMAL " << castResult.collisionResult.normal << endl;
    //     } else {
    //         cerr << "MISS!" << endl;
    //     }
    // }

    Colour colourFromLight(0);

    if (light->isAreaLight) {
        Colour averageColour(0);

        for (int i = 0; i < areaLightSampleDimension; i++) {
            for (int j = 0; j < areaLightSampleDimension; j++) {
                Colour sampleColour = sampleColourFromAreaLight(primaryCast, light, i, j);
                averageColour = averageColour + sampleColour;
            }
        }

        averageColour = (1.0 / (areaLightSampleDimension * areaLightSampleDimension)) * averageColour;
        colourFromLight = averageColour;
    } else { // Point Light
        colourFromLight = calculateColourFromPointLight(primaryCast, light);
    }

    return colourFromLight;
}