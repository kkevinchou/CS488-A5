#include "primitive.hpp"
#include <iostream>

int Primitive::nextId = 0;

Primitive::Primitive() {
    id = nextId++;
}

Primitive::~Primitive()
{
}

Sphere::Sphere() : m_nonhierSphere(Point3D(0, 0, 0), 1) {
}

Cube::Cube() : m_nonhierBox(Point3D(0, 0, 0), 1) {
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
}
