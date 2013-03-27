#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "scene.hpp"
#include "mesh.hpp"
#include "algebra.hpp"
#include <list>
#include "primitive.hpp"

using namespace std;

struct collision_result {
    Point3D point;
    Vector3D normal;
    double hitDistance;
    const PhongMaterial *surfaceMaterial;

    collision_result() {
        surfaceMaterial = NULL;
        hitDistance = -1;
    }
};

class Collider {
public:
    Collider(const SceneNode *root);
    list<collision_result> getCollisionData(const Point3D& pos, const Vector3D& dir) const;
private:
    const SceneNode *root;

    bool hitDistanceTooClose(const collision_result &cr) const;
    list<collision_result> getCollisionData(const Point3D& pos, const Vector3D& dir, const SceneNode *node, Matrix4x4 trans, Matrix4x4 itrans) const;

    list<collision_result> nonhierSphereSolver(NonhierSphere *nhSphere, const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> nonhierBoxSolver(NonhierBox *nhBox, const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> meshSolver(Mesh *mesh, const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> sphereSolver(Sphere *sphere, const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> cubeSolver(Cube *cube, const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> coneSolver(const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> torusSolver(const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> cylinderSolver(const Point3D& pos, const Vector3D& dir) const;
};

#endif