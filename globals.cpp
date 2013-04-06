#include "worker.hpp"
#include "globals.hpp"

unsigned short port = 6090;
Worker worker;
TextureManager textureManager;

bool superSampling = false;
int sampleDimension = 4;

int reflectionMaxRayRecursionDepth = 2;
int reflectionNumDistributedRays = 1;

double const EPSILON = 1.0e-8;
bool printProgress = true;
bool debug = true;
int areaLightSampleDimension = 10;

// int debugX = 242;
int debugX = 300;
int debugY = 230;