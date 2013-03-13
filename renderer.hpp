#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "raycaster.hpp"

class Renderer {
public:
     Renderer(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const list<Light*>& lights);

     vector<double> render(int x, int y, bool superSampling = false, int sampleDimension = 2) const;
private:
     SceneNode* root;
     // Where to output the image
     const std::string& filename;
     // Image size
     int width;
     int height;
     // Viewing parameters
     const Point3D& eye;
     const Vector3D& view;
     const Vector3D& up;
     double fov;
     // Lighting parameters
     const Colour& ambient;
     const std::list<Light*>& lights;
     Vector3D m_view;
     Vector3D m_up;
     Vector3D m_side;
     Background bg;
     RayCaster rayCaster;
};

#endif