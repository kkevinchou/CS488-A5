#include "raycaster.hpp"
#include <math.h>
#include <algorithm>
#include <iostream>

extern bool maxRayRecursionDepth;

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
        if (distSq < 1.0e-8) {
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

cast_result RayCaster::recursiveColourCast(const Point3D &pos, const Vector3D &dir, int recursionDepth) const {
    cast_result primaryCast = cast(pos, dir);

    if (!primaryCast.hit) {
        return primaryCast;
    }

    const PhongMaterial *phongMaterial = primaryCast.collisionResult.phongMaterial;
    Colour finalColour(0);

    for (list<Light *>::const_iterator it = lights.begin(); it != lights.end(); it++) {
        Colour lightColour = shadeFromLight(primaryCast, (*it));
        finalColour = finalColour + lightColour;
    }

    if (recursionDepth < maxRecursionDepth) {
        Point3D collisionPoint = primaryCast.collisionResult.point;
        Vector3D collisionNormal = primaryCast.collisionResult.normal;

        Vector3D reflectionDirection = (-2 * (dir.dot(collisionNormal)) * collisionNormal) + dir;
        reflectionDirection.normalize();

        cast_result recursiveCast = recursiveColourCast(collisionPoint, reflectionDirection, recursionDepth + 1);

        if (recursiveCast.hit) {
            Colour reflectionColour = recursiveCast.finalColour;
            double refCoef = 0.5;
            finalColour = ((1 - refCoef) * finalColour) + (refCoef * reflectionColour);
        }
    }

    primaryCast.finalColour = finalColour + ambient * phongMaterial->get_diffuse();

    return primaryCast;
}

Colour RayCaster::shadeFromLight(struct cast_result primaryCast, const Light *light) const {
    Colour colourFromLight(0);

    cast_result castResult;
    Point3D position = primaryCast.collisionResult.point;
    const PhongMaterial *phongMaterial = primaryCast.collisionResult.phongMaterial;

    // Casting the shadow ray, if it doesn't hit something on the
    // way to the light, then the light isn't being occluded
    castResult = cast(position, light->position - position);
    if (!castResult.hit) {
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
        Colour materialPropertiesColour = phongMaterial->get_diffuse();

        if (lightDotNormal > 0.0) {
            materialPropertiesColour = materialPropertiesColour + (pow(rDotEye, phongMaterial->get_shininess())) / normal.dot(lightVec) * phongMaterial->get_spec();
            colourFromLight = light->colour * materialPropertiesColour * energyIn;
        }
    }

    return colourFromLight;
}