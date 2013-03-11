// Name: Chi Ming Chou
// Username: cm2chou
// Userid: 20309042

#ifndef CS488_A2_HPP
#define CS488_A2_HPP

#include "algebra.hpp"
#include <iostream>

using namespace std;

// You should implement these functions, and use them from viewer.cpp

// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.
Matrix4x4 rotation(double angle, char axis);

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& displacement);

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& scale);

Matrix4x4 axisRotation(const Vector3D& axis);

Matrix4x4 columnOrderedMatrix(const Matrix4x4& matrix);

#endif
