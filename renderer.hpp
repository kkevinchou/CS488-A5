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
               double fov,
               // Lighting parameters
               const Colour& ambient,
               const list<Light*>& lights);

     vector<double> render(int x, int y, bool superSampling = false, int sampleDimension = 2);
private:
     SceneNode* root;
     // Where to output the image
     const std::string& filename;
     // Image size
     int width;
     int height;
     // Viewing parameters
     double fov;
     // Lighting parameters
     const Colour& ambient;
     const std::list<Light*>& lights;
     Background bg;
     RayCaster rayCaster;
};

#endif