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
};

class RayCaster {
public:
    RayCaster(const Point3D& eye, const Background& bg, const SceneNode *root, const list<Light *> &lights, const Colour &ambient);
    cast_result cast(const Point3D &pos, const Vector3D &dir) const;
    cast_result cast2(const Point3D &pos, const Vector3D &dir) const;

    Colour shade(struct cast_result primaryCast, const Light *light) const;
    Colour shade(struct cast_result primaryCast) const;
private:
    const Point3D &eye;
    const Background &bg;
    const list<Light *> &lights;
    const Colour &ambient;
    const SceneNode *root;
    Collider collider;
};

#endif