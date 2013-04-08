#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "worker.hpp"
#include "algebra.hpp"
#include "tween.hpp"
#include "texturemanager.hpp"

extern TextureManager textureManager;

extern unsigned short port;
extern Worker worker;

extern bool superSampling;
extern int sampleDimension;

extern int reflectionMaxRayRecursionDepth;
extern int reflectionNumDistributedRays;

extern const double EPSILON;
extern bool printProgress;
extern bool debug;
extern int areaLightSampleDimension;

extern int debugX;
extern int debugY;

extern Point3D mEye;
extern Vector3D mView;
extern Vector3D mUp;
extern Vector3D mSide;
extern list<Tween*> mTweens;

extern double animLength;
extern int fps;
extern Vector3D moveVecPerFrame;


#endif