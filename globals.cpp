#include "worker.hpp"
#include "globals.hpp"

unsigned short port = 6090;
Worker worker;
bool superSampling = false;
int sampleDimension = 4;
int maxRayRecursionDepth = 1;
int numDistributedRays = 1;
double const EPSILON = 1.0e-8;
bool printProgress = true;
bool debug = false;