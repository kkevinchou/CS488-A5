#include <iostream>
#include <math.h>
#include "polyroots.hpp"
#include "collider.hpp"
#include "globals.hpp"

Collider::Collider(const SceneNode *root) : root(root) {
}

list<collision_result> Collider::getCollisionData(const Point3D& pos, const Vector3D& dir) const {
    Matrix4x4 trans;
    Matrix4x4 itrans;

    list<collision_result> hits = getCollisionData(pos, dir, root, trans, itrans);
    for (list<collision_result>::iterator it = hits.begin(); it != hits.end(); ++it) {
        it->point = it->point - (EPSILON * dir);
        it->hitDistance = it->point.dist(pos);
    }

    list<collision_result>::iterator it = hits.begin();
    while (it != hits.end()) {
        if (hitDistanceTooClose(*it)) {
            hits.erase(it++);
        } else {
            it++;
        }
    }

    return hits;
}

bool Collider::hitDistanceTooClose(const collision_result &cr) const {
    return (cr.hitDistance < EPSILON);
}

list<collision_result> Collider::getCollisionData(const Point3D& pos, const Vector3D& dir, const SceneNode* node, Matrix4x4 trans, Matrix4x4 itrans) const {
    list<collision_result> allHits;

    trans = node->get_transform() * trans;
    itrans = node->get_inverse() * itrans;

    Point3D tpos = itrans * pos;
    Vector3D tdir = itrans * dir;
    tdir.normalize();

    if (node->is_geometry()) {
        const GeometryNode *g = static_cast<const GeometryNode *>(node);
        list<collision_result> newHits;

        switch (g->get_type()) {
            case Primitive::NONHIERSPHERE:
                {
                    NonhierSphere *nhSphere = static_cast<NonhierSphere *>(g->get_primitive());
                    newHits = nonhierSphereSolver(nhSphere, tpos, tdir);
                    break;
                }
            case Primitive::SPHERE:
                {
                    Sphere *sphere = static_cast<Sphere *>(g->get_primitive());
                    newHits = sphereSolver(sphere, tpos, tdir);
                    break;
                }
            case Primitive::CUBE:
                {
                    Cube *cube = static_cast<Cube *>(g->get_primitive());
                    newHits = cubeSolver(cube, tpos, tdir);
                    break;
                }
            case Primitive::NONHIERBOX:
                {
                    NonhierBox *nhBox = static_cast<NonhierBox *>(g->get_primitive());
                    newHits = nonhierBoxSolver(nhBox, tpos, tdir);
                    break;
                }
            case Primitive::MESH:
                {
                    Mesh *mesh = static_cast<Mesh *>(g->get_primitive());
                    newHits = meshSolver(mesh, tpos, tdir, true);
                    break;
                }
            default:
                cerr << "unhandled primitive type" << endl;
                break;
        }

        const PhongMaterial *surfaceMaterial = static_cast<const PhongMaterial *>(g->get_material());
        for (list<collision_result>::iterator it = newHits.begin(); it != newHits.end(); ++it) {
            it->surfaceMaterial = surfaceMaterial;
        }

        allHits.insert(allHits.end(), newHits.begin(), newHits.end());
    }

    list<SceneNode *> children = node->get_children();
    for (list<SceneNode *>::const_iterator it = children.begin(); it != children.end(); ++it) {
        list<collision_result> childHits = getCollisionData(pos, dir, *it, trans, itrans);
        allHits.insert(allHits.end(), childHits.begin(), childHits.end());
    }

    for (list<collision_result>::iterator it = allHits.begin(); it != allHits.end(); ++it) {
        it->point = node->get_transform() * it->point;
        it->normal = node->get_inverse().transpose() * it->normal;
        it->normal.normalize();
    }

    return allHits;
}

list<collision_result> Collider::sphereSolver(Sphere *sphere, const Point3D& pos, const Vector3D& dir) const {
    return nonhierSphereSolver(&sphere->m_nonhierSphere, pos, dir);
}

list<collision_result> Collider::cubeSolver(Cube *cube, const Point3D& pos, const Vector3D& dir) const {
    return nonhierBoxSolver(&cube->m_nonhierBox, pos, dir);
}

list<collision_result> Collider::nonhierSphereSolver(NonhierSphere *nhSphere, const Point3D& pos, const Vector3D& dir) const {
    double a = dir.dot(dir);
    double b = (pos - nhSphere->get_position()).dot(dir) * 2;
    double c = (pos - nhSphere->get_position()).dot(pos - nhSphere->get_position()) - (nhSphere->get_radius() * nhSphere->get_radius());

    double roots[2];
    int quadResult = quadraticRoots(a, b, c, roots);

    list<collision_result> hits;

    double minRoot = INFINITY;

    for (int i = 0; i < quadResult; i++) {
        if (roots[i] < 0) continue;
        if (roots[i] < minRoot) {
            minRoot = roots[i];
        }
    }

    if (minRoot != INFINITY) {
        collision_result hit;
        hit.point = pos + (minRoot * dir);
        hit.normal = (hit.point - nhSphere->get_position());
        hit.normal.normalize();

        hits.push_back(hit);
    }

    return hits;
}

list<collision_result> Collider::nonhierBoxSolver(NonhierBox *nhBox, const Point3D& pos, const Vector3D& dir) const {
    double tMax =  INFINITY;
    double tMin = -INFINITY;

    Point3D boxPosition = nhBox->get_position();
    double size = nhBox->get_size();

    collision_result hit;
    list<collision_result> hits;

    for (int i = 0; i < 3; i++) {
        double t1 = (boxPosition[i] - pos[i]) / dir[i];
        double t2 = (boxPosition[i] + size - pos[i]) / dir[i];

        if (t1 > t2) {
            swap(t1, t2);
        }

        if (t2 < tMax) {
            tMax = t2;
        }

        if (t1 > tMin) {
            tMin = t1;

            if (i == 0) {
                hit.normal = Vector3D(-dir[0], 0, 0);
            } else if (i == 1) {
                hit.normal = Vector3D(0, -dir[1], 0);
            } else if (i == 2) {
                hit.normal = Vector3D(0, 0, -dir[2]);
            }
            hit.normal.normalize();
        }

        if ((tMin > tMax) || tMax < 0) {
            return hits;
        }
    }

    hit.point = pos + (tMin - EPSILON) * dir;
    hits.push_back(hit);

    return hits;
}

list<collision_result> Collider::meshSolver(Mesh *mesh, const Point3D& pos, const Vector3D& dir, bool useAABB) const {
    if (useAABB) {
        list<collision_result> aabbHits = nonhierBoxSolver(mesh->aabb, pos, dir);
        // return aabbHits;
        if (aabbHits.size() == 0) {
            return aabbHits;
        }
    }

    list<collision_result> hits;
    vector<struct face_plane> facePlanes = mesh->facePlanes;

    for (unsigned int i = 0; i < facePlanes.size(); i++) {
        struct face_plane f = facePlanes[i];
        unsigned int numPoints = f.points.size();

        int refPointIndex = 1;
        Point3D refPoint = f.points[refPointIndex];
        Vector3D refNormal = (f.points[refPointIndex + 1] - refPoint).cross(f.points[refPointIndex - 1] - refPoint);
        refNormal.normalize();

        double t = (refPoint - pos).dot(refNormal) / (dir.dot(refNormal));

        if (t <= 0) {
            continue;
        }

        Point3D intersectPoint = pos + t * dir;

        collision_result hit;
        hit.point = intersectPoint;
        hit.normal = refNormal;

        bool passesPoints = true;
        for (unsigned int j = 0; j < numPoints; j++) {
            unsigned int nextIndex = (j + 1) % numPoints;

            Point3D point = f.points[j];
            Vector3D windingVec = f.points[nextIndex] - point;

            Vector3D refVector = refNormal.cross(windingVec);
            refVector.normalize();
            if ((intersectPoint - point).dot(refVector) <= 0) {
                passesPoints = false;
                break;
            }
        }

        if (passesPoints) {
            hits.push_back(hit);
        }
    }

    return hits;
}