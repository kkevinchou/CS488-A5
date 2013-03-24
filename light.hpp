#ifndef CS488_LIGHT_HPP
#define CS488_LIGHT_HPP

#include "algebra.hpp"
#include <iosfwd>

// Represents a simple point light.
struct Light {
  Light();

  Colour colour;
  Point3D position;
  double falloff[3];
  bool isAreaLight;
  Vector3D rightVec;
  Vector3D downVec;
};

std::ostream& operator<<(std::ostream& out, const Light& l);

#endif
