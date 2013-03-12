#include "a4.hpp"
#include "image.hpp"
#include "background.hpp"
#include "collider.hpp"
#include "worker.hpp"
#include "dist.hpp"
#include "renderer.hpp"
#include "coordinator.hpp"

bool debug = false;
bool printProgress = true;

extern unsigned short port;

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
    char * run_type = getenv ("RUN_TYPE");

    Renderer r(root, filename, width, height, eye, view, up, fov, ambient, lights);

    if (string(run_type) == "WORKER") {
        cerr << "WORKER START" << endl;
        wait(r, width, height);
        cerr << "WORKER END" << endl;
    } else if (string(run_type) == "COORDINATOR") {
        go(width, height, filename);
    } else {
        Image img(width, height, 3);

        int percentage = 0;
        clock_t t = clock();

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                vector<double> colour = r.render(x, y);
                img(x, y, 0) = colour[0];
                img(x, y, 1) = colour[1];
                img(x, y, 2) = colour[2];

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
        img.savePng(filename);
    }
}
