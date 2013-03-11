#include "a4.hpp"
#include "image.hpp"
#include "background.hpp"
#include "collider.hpp"
#include "worker.hpp"
#include "coordinator.hpp"

bool debug = false;
bool printProgress = true;

Renderer::Renderer(// What to render
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
               const list<Light*>& lights)
: root(root),
filename(filename),
width(width),
height(height),
eye(eye),
view(view),
up(up), fov(fov),
ambient(ambient),
lights(lights),
bg(width, height),
rayCaster(eye, bg, root, lights, ambient) {
    m_view = view;
    m_up = up;
    m_side = view.cross(up);
    }

vector<double> Renderer::render(int x, int y) {
    bool superSampling = false;
    double sampleDimension = 16.0;

    Vector3D dir;
    Colour c(0);
    int hitCount = 0;
    int percentage = 0;
    clock_t t = clock();

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

    if (printProgress) {
        cerr << ". 100\% done" << endl;
    }

    t = clock() - t;
    cerr << "Took " << ((float)t)/CLOCKS_PER_SEC << " seconds to complete" << endl;
    cerr << "HIT COUNT: " << hitCount << endl;

    vector<double> results;
    results.push_back(c.R());
    results.push_back(c.G());
    results.push_back(c.B());

    cerr << "generated colour: " << c << endl;
    return results;
}

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

    if (string(run_type) == "WORKER") {
        Renderer r(root, filename, width, height, eye, view, up, fov, ambient, lights);
        wait(r);
    } else if (string(run_type) == "COORDINATOR") {
        int workerFd = setupSocketAndReturnDescriptor(string("gl27").c_str(), 6013);
        if (workerFd < 0) {
            cerr << "Connection Failed" << endl;
        }

        double sendData[3] = {300, 400, 0};

        for (int i = 0; i < 3; i++) {
            sendDouble(workerFd, sendData[i]);
        }

        sleep(10);
    } else {
        Vector3D m_view      = view;
        Vector3D m_up        = up;
        Vector3D m_side      = view.cross(up);

        m_side.normalize();
        m_view.normalize();
        m_up.normalize();

        Background bg(width, height);
        RayCaster rayCaster(eye, bg, root, lights, ambient);

        Image img(width, height, 3);

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
}
