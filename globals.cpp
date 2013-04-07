#include "worker.hpp"
#include "algebra.hpp"
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

Point3D mEye;
Vector3D mView;
Vector3D mUp;
Vector3D mSide;

double animLength = 1.0;
int fps = 1;
Vector3D moveVecPerFrame;