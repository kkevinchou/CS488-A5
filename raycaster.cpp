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


double getU(double t)
{
    return t/(2*PI);
}

double getV(double s)
{
    return  (cos(s) + 1)/2;
}

// returns a rand numb between 0 and 1
double getRandom()
{
    return ((double)rand())/(double)RAND_MAX;
}

Vector3D perturbVector(const Point3D &pos, const Vector3D &dir) {
    Point3D pointOnSphere = pos + dir;

    double t = acos(pointOnSphere[2] - pos[2]);
    double s = atan2((pointOnSphere[1] - pos[1]), (pointOnSphere[0] - pos[0]));

    double degree = 0.5*PI/180;

    double s0 = s-degree;
    double s1 = s+degree;
    double t0 = t-degree;
    double t1 = t+degree;

    double randS = s0 + getRandom() * (s1 - s0);
    double randT = t0 + getRandom() * (t1 - t0);

    Point3D p(cos(randS)*sin(randT) + pos[0],
                sin(randS)*sin(randT) + pos[1],
                cos(randT) + pos[2]);

    Vector3D yufDir = p - pos;
    yufDir.normalize();

    return yufDir;
}

Vector3D perturbVector2(const Vector3D &dir, double circleRadius) {
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
    Colour finalColour(0);

    for (list<Light *>::const_iterator it = lights.begin(); it != lights.end(); it++) {
        bool hitLight;
        Colour lightColour = shadeFromLight(primaryCast, (*it), hitLight);
        if (hitLight) {
            primaryCast.hitLight = true;
        }
        finalColour = finalColour + lightColour;
    }

    if (recursionDepth < maxRecursionDepth) {
        Point3D collisionPoint = primaryCast.collisionResult.point;
        Vector3D collisionNormal = primaryCast.collisionResult.normal;

        Vector3D reflectionDirection = (-2 * (dir.dot(collisionNormal)) * collisionNormal) + dir;
        reflectionDirection.normalize();

        Colour reflectionColour(0);

        int numHits = 0;

        for (int i = 0; i < numDistributedRays; i++) {
            Vector3D stochasticDirection = perturbVector2(reflectionDirection, glossRadius);
            cast_result recursiveCast = recursiveColourCast(collisionPoint, stochasticDirection, recursionDepth + 1);

            if (recursiveCast.hit) {
                reflectionColour = reflectionColour + recursiveCast.finalColour;
                numHits++;
            }
        }

        if (numHits > 0) {
            reflectionColour = (1.0 / numHits) * reflectionColour;
            finalColour = ((1 - reflectionCoefficient) * finalColour) + (reflectionCoefficient * reflectionColour);
        }

    }

    primaryCast.finalColour = finalColour + ambient * surfaceMaterial->get_diffuse();

    return primaryCast;
}

Colour RayCaster::shadeFromLight(struct cast_result primaryCast, const Light *light, bool &hitLight) const {
    Colour colourFromLight(0);

    cast_result castResult;
    Point3D position = primaryCast.collisionResult.point;
    const PhongMaterial *surfaceMaterial = primaryCast.collisionResult.surfaceMaterial;

    // Casting the shadow ray, if it doesn't hit something on the
    // way to the light, then the light isn't being occluded
    castResult = cast(position, light->position - position);
    if (!castResult.hit || light->position.dist(position) < castResult.collisionResult.hitDistance) {
        if (debug) {
            if (castResult.hit) {
                cerr << "HIT NORMAL " << castResult.collisionResult.normal << endl;
                cerr << "LIGHT POSITION " << light->position << endl;
                cerr << "LIGHT DISTANCE " << light->position.dist(position) << endl;
                cerr << "HIT POSITION " << castResult.collisionResult.point << endl;
                cerr << "HIT DISTANCE " << castResult.collisionResult.hitDistance << endl;
                cerr << "START POSITION " << primaryCast.collisionResult.point << endl;
            }
        }

        hitLight = true;
        double distSq = position.distSq(light->position);
        Vector3D lightVec = light->position - position;
        lightVec.normalize();
        Vector3D normal = primaryCast.collisionResult.normal;

        double lightDotNormal = max(lightVec.dot(normal), 0.0);
        const double *falloff = light->falloff;
        double energyIn = lightDotNormal;
        energyIn /= (falloff[0] + falloff[1] * sqrt(distSq) + falloff[2] * distSq);

        Vector3D r = (-1 * lightVec) + (2 * lightDotNormal * normal);
        Vector3D eyeVec = eye - position;
        eyeVec.normalize();

        double rDotEye = max(r.dot(eyeVec), 0.0);
        Colour materialPropertiesColour = surfaceMaterial->get_diffuse();

        if (lightDotNormal > 0.0) {
            materialPropertiesColour = materialPropertiesColour + (pow(rDotEye, surfaceMaterial->get_shininess())) / normal.dot(lightVec) * surfaceMaterial->get_spec();
            colourFromLight = light->colour * materialPropertiesColour * energyIn;
        }
    } else {
        if (debug) {
            cerr << castResult.collisionResult.normal << endl;
        }

        hitLight = false;
    }

    return colourFromLight;
}