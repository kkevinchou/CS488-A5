#include "worker.hpp"
#include "globals.hpp"

unsigned short port = 6090;
Worker worker;
bool superSampling = false;
int sampleDimension = 4;
int maxRayRecursionDepth = 0;
int numDistributedRays = 1;
double const EPSILON = 1.0e-8;
bool printProgress = false;
bool debug = false;
int areaLightSampleDimension = 3;

int debugX = 280;
int debugY = 280;