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
    const PhongMaterial *phongMaterial;
};

class Collider {
public:
    Collider(const SceneNode *root);
    list<collision_result> getCollisionData(const Point3D& pos, const Vector3D& dir) const;
private:
    const SceneNode *root;

    list<collision_result> getCollisionData(const Point3D& pos, const Vector3D& dir, const SceneNode *node, Matrix4x4 trans, Matrix4x4 itrans) const;

    list<collision_result> nonhierSphereSolver(NonhierSphere *nhs, const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> nonhierBoxSolver(NonhierBox *nhb, const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> meshSolver(Mesh *mesh, const Point3D& pos, const Vector3D& dir, bool useAABB) const;
    list<collision_result> sphereSolver(Sphere *s, const Point3D& pos, const Vector3D& dir) const;
    list<collision_result> cubeSolver(Cube *c, const Point3D& pos, const Vector3D& dir) const;

    list<collision_result> boundingBoxSolver(NonhierBox *nhb, const Vector3D &dimension, const Point3D& pos, const Vector3D& dir) const;
};

#endif