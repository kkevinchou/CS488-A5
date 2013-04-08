#include "worker.hpp"
#include "algebra.hpp"
#include "globals.hpp"

unsigned short port = 6092;
Worker worker;
TextureManager textureManager;

bool superSampling = false;
int sampleDimension = 1;

int reflectionMaxRayRecursionDepth = 1;
int reflectionNumDistributedRays = 3;

double const EPSILON = 1.0e-8;
bool printProgress = true;
bool debug = false;
int areaLightSampleDimension = 3;

// int debugX = 242;
int debugX = 300;
int debugY = 230;

Point3D mEye;
Vector3D mView;
Vector3D mUp;
Vector3D mSide;
list<Tween*> mTweens;

double animLength = 0;
int fps = 0;