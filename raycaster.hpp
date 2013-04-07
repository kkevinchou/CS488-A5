#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include "algebra.hpp"
#include "background.hpp"
#include "scene.hpp"
#include "collider.hpp"
#include "light.hpp"

using namespace std;

struct ray {
    Point3D pos;
    Vector3D dir;
};

struct cast_result {
    collision_result collisionResult;
    bool hit;
    Colour finalColour;

    cast_result() {
        hit = false;
    };
};

class RayCaster {
public:
    RayCaster(const Background& bg, const SceneNode *root, const list<Light *> &lights, const Colour &ambient);
    cast_result colourCast(const Point3D &pos, const Vector3D &dir);

private:
    Colour shade(struct cast_result primaryCast) const;
    Colour calculateColourFromPointLight(struct cast_result primaryCast, const Light *light) const;
    Colour sampleColourFromAreaLight(struct cast_result primaryCast, const Light *light, int cellX, int cellY) const;
    Colour shadeFromLight(struct cast_result primaryCast, const Light *light) const;
    cast_result recursiveColourCast(const Point3D &pos, const Vector3D &dir, int recursionDepth) const;

    double getRandom() const;
    Vector3D perturbVector(const Vector3D &dir, double circleRadius) const;

    cast_result cast(const Point3D &pos, const Vector3D &dir) const;
    const Background &bg;
    const SceneNode *root;
    const list<Light *> &lights;
    const Colour &ambient;
    Collider collider;

    int maxRecursionDepth;

    Point3D localEye;
};

#endif