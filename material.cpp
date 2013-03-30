#include "material.hpp"
#include "globals.hpp"
#include "texturemanager.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double glossiness, double reflectivity)
  : m_kd(kd), m_ks(ks), m_shininess(shininess), m_glossiness(glossiness), m_reflectivity(reflectivity)
{
}

PhongMaterial::~PhongMaterial()
{
}

TextureMaterial::TextureMaterial(const string& textureFile, const Colour& ks, double shininess, double glossiness, double reflectivity)
  : PhongMaterial(Colour(), ks, shininess, glossiness, reflectivity), textureFile(textureFile) {
}

TextureMaterial::~TextureMaterial()
{
}

Colour TextureMaterial::get_texture_colour(const Point2D &textureCoordinates) const {
    return textureManager.getTextureColour(textureFile, textureCoordinates);
}