#include <iostream>
#include <math.h>
#include "polyroots.hpp"
#include "collider.hpp"

extern bool debug;

Collider::Collider(const SceneNode *root) : root(root) {
}

list<collision_result> Collider::getCollisionData(const Point3D& pos, const Vector3D& dir) const {
    Matrix4x4 trans;
    Matrix4x4 itrans;
    return getCollisionData(pos, dir, root, trans, itrans);
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
                    NonhierSphere *p = static_cast<NonhierSphere *>(g->get_primitive());
                    newHits = nonhierSphereSolver(p, tpos, tdir);
                    break;
                }
            case Primitive::SPHERE:
                {
                    Sphere *p = static_cast<Sphere *>(g->get_primitive());
                    newHits = sphereSolver(p, tpos, tdir);
                    break;
                }
            case Primitive::CUBE:
                {
                    Cube *c = static_cast<Cube *>(g->get_primitive());
                    newHits = cubeSolver(c, tpos, tdir);
                    break;
                }
            case Primitive::NONHIERBOX:
                {
                    NonhierBox *p = static_cast<NonhierBox *>(g->get_primitive());
                    newHits = nonhierBoxSolver(p, tpos, tdir);
                    break;
                }
            case Primitive::MESH:
                {
                    Mesh *p = static_cast<Mesh *>(g->get_primitive());
                    newHits = meshSolver(p, tpos, tdir, true);
                    break;
                }
            default:
                cerr << "unhandled primitive type" << endl;
                break;
        }

        const PhongMaterial *m = static_cast<const PhongMaterial *>(g->get_material());
        for (list<collision_result>::iterator it = newHits.begin(); it != newHits.end(); ++it) {
            it->phongMaterial = m;
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

list<collision_result> Collider::sphereSolver(Sphere *s, const Point3D& pos, const Vector3D& dir) const {
    return nonhierSphereSolver(&s->m_nonhierSphere, pos, dir);
}

list<collision_result> Collider::cubeSolver(Cube *c, const Point3D& pos, const Vector3D& dir) const {
    return nonhierBoxSolver(&c->m_nonhierBox, pos, dir);
}

list<collision_result> Collider::nonhierSphereSolver(NonhierSphere *nhs, const Point3D& pos, const Vector3D& dir) const {
    double a = dir.dot(dir);
    double b = (pos - nhs->get_position()).dot(dir) * 2;
    double c = (pos - nhs->get_position()).dot(pos - nhs->get_position()) - (nhs->get_radius() * nhs->get_radius());

    double roots[2];
    int quadResult = quadraticRoots(a, b, c, roots);

    list<collision_result> hits;
    for (int i = 0; i < quadResult; i++) {
        if (roots[i] < 0) continue;
        collision_result hit;
        hit.point = pos + (roots[i] * dir);
        hit.normal = (hit.point - nhs->get_position());
        hit.normal.normalize();

        hits.push_back(hit);
    }

    return hits;
}

list<collision_result> Collider::boundingBoxSolver(NonhierBox *nhb, const Vector3D &dimension, const Point3D& pos, const Vector3D& dir) const {
    double tMax =  INFINITY;
    double tMin = -INFINITY;

    Point3D boxPosition = nhb->get_position();

    collision_result hit;
    list<collision_result> hits;

    for (int i = 0; i < 3; i++) {
        double t1 = (boxPosition[i] - pos[i]) / dir[i];
        double t2 = (boxPosition[i] + max(max(dimension[0], dimension[1]), dimension[2]) - pos[i]) / dir[i];

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

    hit.point = pos + (tMin - 0.01) * dir;
    hits.push_back(hit);

    return hits;
}

list<collision_result> Collider::nonhierBoxSolver(NonhierBox *nhb, const Point3D& pos, const Vector3D& dir) const {
    double tMax =  INFINITY;
    double tMin = -INFINITY;

    Point3D boxPosition = nhb->get_position();
    double size = nhb->get_size();

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

    hit.point = pos + (tMin - 0.01) * dir;
    hits.push_back(hit);

    return hits;
}

list<collision_result> Collider::meshSolver(Mesh *mesh, const Point3D& pos, const Vector3D& dir, bool useAABB) const {
    if (useAABB) {
        list<collision_result> aabbHits = boundingBoxSolver(mesh->aabb, mesh->dimension, pos, dir);
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