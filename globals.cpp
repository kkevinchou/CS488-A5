#include "worker.hpp"
#include "globals.hpp"

unsigned short port = 6090;
Worker worker;
bool superSampling = false;
int sampleDimension = 2;
int maxRayRecursionDepth = 0;
double glossRadius = 0.03;
int numDistributedRays = 4;
double reflectionCoefficient = 0.2;
double const EPSILON = 1.0e-8;
bool printProgress = true;