#include "worker.hpp"

unsigned short port = 6090;
Worker worker;
bool superSampling = true;
int sampleDimension = 2;
int maxRayRecursionDepth = 0;
double glossRadius = 0.03;
int numDistributedRays = 4;
double reflectionCoefficient = 0.2;