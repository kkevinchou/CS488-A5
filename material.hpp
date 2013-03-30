#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

#include <string>

using namespace std;

class Material {
public:
  virtual ~Material();

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double glossiness, double reflectivity);
  virtual ~PhongMaterial();

  Colour get_diffuse() const {
    return m_kd;
  }

  virtual Colour get_texture_colour(const Point2D &textureCoordinates) const {
    cerr << "ERROR, called get_texture_colour on non-texture material" << endl;
    return Colour();
  }

  Colour get_spec() const {
    return m_ks;
  }

  double get_shininess() const {
    return m_shininess;
  }

  double get_glossiness() const {
    return m_glossiness;
  }

  double get_reflectivity() const {
    return m_reflectivity;
  }

  double is_reflective() const {
    return m_reflectivity > 0;
  }

  virtual bool isTexture() const {
    return false;
  }
protected:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
  double m_glossiness;
  double m_reflectivity;
};

class TextureMaterial : public PhongMaterial {
public:
  TextureMaterial(const string& textureFile, const Colour& ks, double shininess, double glossiness, double reflectivity);
  virtual ~TextureMaterial();

  virtual Colour get_texture_colour(const Point2D &textureCoordinates) const;

  virtual bool isTexture() const {
    return true;
  }
private:
  string textureFile;

};

#endif
