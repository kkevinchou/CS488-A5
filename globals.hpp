#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "worker.hpp"

extern unsigned short port;
extern Worker worker;
extern bool superSampling;
extern int sampleDimension;
extern int maxRayRecursionDepth;
extern double glossRadius;
extern int numDistributedRays;
extern double reflectionCoefficient;
extern const double EPSILON;
extern bool printProgress;
extern bool debug;
extern int areaLightSampleDimension;

#endif