#include "renderer.hpp"
#include "globals.hpp"

Renderer::Renderer(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               double fov,
               // Lighting parameters
               const Colour& ambient,
               const list<Light*> &lights)
: root(root),
filename(filename),
width(width),
height(height),
fov(fov),
ambient(ambient),
lights(lights),
bg(width, height),
rayCaster(bg, root, lights, ambient) {
}

vector<double> Renderer::render(int x, int y, bool superSampling, int sampleDimension) {
    Vector3D dir;
    Colour c(0);

    if (!superSampling) {
        dir = (x / ((double)width) * 2 - 1) *
            tan(fov/2 * M_PI/180.0) *
            ((double)width / (double)height) *
            mSide + (y / (double)height * 2 - 1) *
            tan(fov/2 * M_PI/180.0) *
            -mUp + mView;

        if (x == debugX && y == debugY) {
            debug = true;
        }
        cast_result cr = rayCaster.colourCast(mEye, dir);

        c = (cr.hit) ? cr.finalColour : bg.getPixelColour(x, y);
    } else {
        for (int i = -(double)sampleDimension/2; i < (double)sampleDimension/2; i++) {
            for (int j = -(double)sampleDimension/2; j < (double)sampleDimension/2; j++) {
                dir = ( (x + i/(double)sampleDimension) / ((double)width) * 2 - 1 ) *
                    tan( fov * M_PI / 360.0 ) *
                    ( (double)width / (double)height ) *
                    mSide + ( (y + j/(double)sampleDimension) / (double)height * 2 - 1 ) *
                    tan( fov * M_PI / 360.0 ) *
                    -mUp + mView;
                dir.normalize();

                if (x == debugX && y == debugY) {
                    debug = true;
                }

                cast_result cr = rayCaster.colourCast(mEye, dir);
                c = (cr.hit) ? c + cr.finalColour : c + bg.getPixelColour(x, y);
            }
        }

        c = (1.0 / ((double)sampleDimension * (double)sampleDimension)) * c;
    }

    vector<double> results;
    results.push_back(c.R());
    results.push_back(c.G());
    results.push_back(c.B());

    return results;
}