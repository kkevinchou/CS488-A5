#include "worker.hpp"
#include "globals.hpp"

unsigned short port = 6090;
Worker worker;

bool superSampling = false;
int sampleDimension = 4;

int reflectionMaxRayRecursionDepth = 2;
int reflectionNumDistributedRays = 1;

double const EPSILON = 1.0e-8;
bool printProgress = false;
bool debug = false;
int areaLightSampleDimension = 10;

// int debugX = 242;
int debugX = 250;
int debugY = 250;