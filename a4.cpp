#include "a4.hpp"
#include "image.hpp"
#include "background.hpp"
#include "collider.hpp"
#include <ctime>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
#include <sstream>
#include <map>

bool debug = false;

bool printProgress = true;

void a4_render(// What to render
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
               const list<Light*>& lights
               )
{
    cerr << "RUNNING..........................." << endl;

    Vector3D m_view      = view;
    Vector3D m_up        = up;
    Vector3D m_side      = view.cross(up);

    double m_width = 2 * tan(M_PI * fov / (2 * 180));

    m_side.normalize();
    m_view.normalize();
    m_up.normalize();

    Background bg(width, height);
    RayCaster rayCaster(eye, bg, root, lights, ambient);

    Image img(width, height, 3);

    double focalLength = (double)width / (2 * tan(M_PI * fov / (2 * 180)));

    double offsetX = (double)width / 2;
    double offsetY = (double)height / 2;

    int hitCount = 0;

    int percentage = 0;
    clock_t t = clock();

    bool superSampling = false;
    double sampleDimension = 16.0;

    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
            Vector3D dir;
            Colour c(0);

            if (!superSampling) {
                dir = (x / ((double)width) * 2 - 1) *
                    tan(fov/2 * M_PI/180.0) *
                    ((double)width / (double)height) *
                    m_side + (y / (double)height * 2 - 1) *
                    tan(fov/2 * M_PI/180.0) *
                    -m_up + m_view;

                if (x == 250 && y == 250) {
                    debug = true;
                }
                cast_result cr = rayCaster.cast2(eye, dir);
                if (debug) {
                    debug = false;
                }

                if (cr.hit) {
                    hitCount++;
                }

                c = (cr.hit) ? cr.finalColour : bg.getPixelColour(x, y);
            } else {
                for (int i = -sampleDimension/2; i < sampleDimension/2; i++) {
                    for (int j = -sampleDimension/2; j < sampleDimension/2; j++) {
                        dir = ( (x + i/sampleDimension) / ((double)width) * 2 - 1 ) *
                            tan( fov * M_PI / 360.0 ) *
                            ( (double)width / (double)height ) *
                            m_side + ( (y + j/sampleDimension) / (double)height * 2 - 1 ) *
                            tan( fov * M_PI / 360.0 ) *
                            -m_up + m_view;
                        dir.normalize();

                        cast_result cr = rayCaster.cast2(eye, dir);

                        if (cr.hit) {
                            hitCount++;
                        }

                        c = (cr.hit) ? c + cr.finalColour : c + bg.getPixelColour(x, y);
                    }
                }

                c = (1.0 / (sampleDimension * sampleDimension)) * c;
            }

            img(x, y, 0) = c.R();
            img(x, y, 1) = c.G();
            img(x, y, 2) = c.B();

            if (printProgress) {
                int newPercentage = (double)(x * height + y) * 100 / (width * height);
                if (newPercentage > percentage) {
                      percentage = newPercentage;
                      cerr << ". ";
                      if (percentage % 10 == 0) {
                        cerr << percentage << "\% done" << endl;
                      }
                }
            }
        }
    }

    if (printProgress) {
        cerr << ". 100\% done" << endl;
    }

    t = clock() - t;
    cerr << "Took " << ((float)t)/CLOCKS_PER_SEC << " seconds to complete" << endl;
    cerr << "HIT COUNT: " << hitCount << endl;

    img.savePng(filename);
}
