#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl() const = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double glossiness, double reflectivity);
  virtual ~PhongMaterial();

  virtual void apply_gl() const;

  Colour get_diffuse() const {
    return m_kd;
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
private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
  double m_glossiness;
  double m_reflectivity;
};


#endif
