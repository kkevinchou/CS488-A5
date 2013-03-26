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
            case Primitive::NONHIERSPHERE: {
                NonhierSphere *nhSphere = static_cast<NonhierSphere *>(g->get_primitive());
                newHits = nonhierSphereSolver(nhSphere, tpos, tdir);
                break;
            }
            case Primitive::SPHERE: {
                Sphere *sphere = static_cast<Sphere *>(g->get_primitive());
                newHits = sphereSolver(sphere, tpos, tdir);
                break;
            }
            case Primitive::CUBE: {
                Cube *cube = static_cast<Cube *>(g->get_primitive());
                newHits = cubeSolver(cube, tpos, tdir);
                break;
            }
            case Primitive::NONHIERBOX: {
                NonhierBox *nhBox = static_cast<NonhierBox *>(g->get_primitive());
                newHits = nonhierBoxSolver(nhBox, tpos, tdir);
                break;
            }
            case Primitive::MESH: {
                Mesh *mesh = static_cast<Mesh *>(g->get_primitive());
                newHits = meshSolver(mesh, tpos, tdir);
                break;
            }
            case Primitive::CYLINDER: {
                newHits = cylinderSolver(tpos, tdir);
                break;
            }
            case Primitive::CONE: {
                newHits = coneSolver(tpos, tdir);
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

bool inRange(double checkValue, double min, double max) {
    // return (checkValue <= range1 && checkValue >= range2) || (checkValue <= range2 && checkValue >= range1);
    return (checkValue <= max && checkValue >= min);
}

bool epsilonEquals(double a, double b) {
    return abs(a - b) <= EPSILON;
}

list<collision_result> Collider::cylinderSolver(const Point3D& pos, const Vector3D& dir) const {
    // From: http://www.cl.cam.ac.uk/teaching/1999/AGraphHCI/SMAG/node2.html#SECTION00023200000000000000

    if (dir[2] == 0) {
        list<collision_result> emptyResult;
        return emptyResult;
    }

    double a = (dir[0] * dir[0]) + (dir[1] * dir[1]);
    double b = (2 * pos[0] * dir[0]) + (2 * pos[1] * dir[1]);
    double c = (pos[0] * pos[0]) + (pos[1] * pos[1]) - 1;

    double roots[2];
    int quadResult = quadraticRoots(a, b, c, roots);

    double cylinderHeight = 1.0;
    double radius_2 = 1.0 + EPSILON;

    vector<Point3D> hitPoints;
    double minRoot = INFINITY;
    for (int i = 0; i < quadResult; i++) {
        if (roots[i] < 0) continue;
        if (roots[i] > minRoot) continue;

        Point3D hitPoint = pos + (roots[i] * dir);
        double hitX_2 = hitPoint[0] * hitPoint[0];
        double hitY_2 = hitPoint[1] * hitPoint[1];

        if (inRange(hitPoint[2], 0, cylinderHeight) && (hitX_2 + hitY_2 <= radius_2)) {
            minRoot = roots[i];
            hitPoints.clear();
            hitPoints.push_back(pos + (roots[i] * dir));
        }
    }

    double topRoot = (cylinderHeight - pos[2]) / dir[2];
    Point3D topPoint = pos + (topRoot * dir);
    double topX_2 = topPoint[0] * topPoint[0];
    double topY_2 = topPoint[1] * topPoint[1];

    if ((topRoot >= 0) && (topX_2 + topY_2 <= (1 + EPSILON))) {
        hitPoints.push_back(topPoint);
    }

    double bottomRoot = (0 - pos[2]) / dir[2];
    Point3D bottomPoint = pos + (bottomRoot * dir);
    double bottomX_2 = bottomPoint[0] * bottomPoint[0];
    double bottomY_2 = bottomPoint[1] * bottomPoint[1];

    if ((bottomRoot >= 0) && (bottomX_2 + bottomY_2 <= (1 + EPSILON))) {
        hitPoints.push_back(bottomPoint);
    }

    Point3D closestPoint;
    double minDist = INFINITY;
    for (vector<Point3D>::iterator it = hitPoints.begin(); it != hitPoints.end(); it++) {
        double distance = it->dist(pos);
        if (distance < minDist) {
            minDist = distance;
            closestPoint = *it;
        }
    }

    list<collision_result> hits;

    if (minDist == INFINITY) {
        return hits;
    }

    collision_result hit;
    hit.point = closestPoint;

    if (epsilonEquals(hit.point[2], cylinderHeight)) {
        hit.normal = Vector3D(0, 0, 1);
    } else if (epsilonEquals(hit.point[2], 0)) {
        hit.normal = Vector3D(0, 0, -1);
    } else {
        hit.normal = Vector3D(hit.point[0], hit.point[1], 0);
        hit.normal.normalize();
    }

    hits.push_back(hit);
    return hits;
}

list<collision_result> Collider::coneSolver(const Point3D& pos, const Vector3D& dir) const {
    // From: http://www.cl.cam.ac.uk/teaching/1999/AGraphHCI/SMAG/node2.html#SECTION00023200000000000000

    if (dir[2] == 0) {
        list<collision_result> emptyResult;
        return emptyResult;
    }

    double a = (dir[0] * dir[0]) + (dir[1] * dir[1]) - (dir[2] * dir[2]);
    double b = (2 * pos[0] * dir[0]) + (2 * pos[1] * dir[1]) - (2 * pos[2] * dir[2]);
    double c = (pos[0] * pos[0]) + (pos[1] * pos[1]) - (pos[2] * pos[2]);

    double roots[2];
    int quadResult = quadraticRoots(a, b, c, roots);

    double coneHeight = 1.0;

    vector<Point3D> hitPoints;
    double minRoot = INFINITY;
    for (int i = 0; i < quadResult; i++) {
        if (roots[i] < 0) continue;
        if (roots[i] > minRoot) continue;

        Point3D hitPoint = pos + (roots[i] * dir);
        double hitX_2 = hitPoint[0] * hitPoint[0];
        double hitY_2 = hitPoint[1] * hitPoint[1];
        double coneSliceRadius_2 = (hitPoint[2] + EPSILON) * (hitPoint[2] + EPSILON);

        if (inRange(hitPoint[2], 0, coneHeight) && (hitX_2 + hitY_2 <= coneSliceRadius_2)) {
            if (debug) {
                cerr << "HIT POINT" << hitPoint << endl;
            }
            minRoot = roots[i];
            hitPoints.clear();
            hitPoints.push_back(pos + (roots[i] * dir));
        }
    }

    double bottomRoot = (0 - pos[2]) / dir[2];
    Point3D bottomPoint = pos + (bottomRoot * dir);
    double bottomX_2 = bottomPoint[0] * bottomPoint[0];
    double bottomY_2 = bottomPoint[1] * bottomPoint[1];

    if ((bottomRoot >= 0) && (bottomX_2 + bottomY_2 <= (EPSILON * EPSILON))) {
        hitPoints.push_back(bottomPoint);
    }

    Point3D closestPoint;
    double minDist = INFINITY;
    for (vector<Point3D>::iterator it = hitPoints.begin(); it != hitPoints.end(); it++) {
        double distance = it->dist(pos);
        if (distance < minDist) {
            minDist = distance;
            closestPoint = *it;
        }
    }

    list<collision_result> hits;

    if (minDist == INFINITY) {
        return hits;
    }

    collision_result hit;
    hit.point = closestPoint;

    if (epsilonEquals(hit.point[2], coneHeight)) {
        hit.normal = Vector3D(0, 0, 1);
    } else if (epsilonEquals(hit.point[2], 0)) {
        hit.normal = Vector3D(0, 0, -1);
    } else {
        hit.normal = Vector3D(hit.point[0], hit.point[1], 0);
        hit.normal.normalize();
    }

    hits.push_back(hit);
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

list<collision_result> Collider::meshSolver(Mesh *mesh, const Point3D& pos, const Vector3D& dir) const {
    list<collision_result> aabbHits = nonhierBoxSolver(mesh->aabb, pos, dir);
    // return aabbHits;
    if (aabbHits.size() == 0) {
        return aabbHits;
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