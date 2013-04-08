#include "tween.hpp"
#include "globals.hpp"
#include <iostream>

using namespace std;

Tween::Tween() {
}

Tween::Tween(Type type, double startTime, double endTime, Point3D target) : type(type), startTime(startTime), endTime(endTime), target(target) {
}

void Tween::init() {
    if (type == Tween::TRANSLATE) {
        Vector3D moveVec = target - mEye;
        tweenDelta = (1.0 / (endTime - startTime)) * (1.0 / fps) * moveVec;
        // cerr << "FPS " << fps << endl;
        // cerr << "END TIME " << endTime << endl;
        // cerr << "START TIME " << startTime << endl;
        // cerr << "TARGET " << target << endl;
        // cerr << "MEYE " << mEye << endl;
        // cerr << "TWEEN DELTA " << tweenDelta << endl;
    }
    else if (type == Tween::ROTATE) {
        Vector3D rotVec = target - Point3D();

        double xRot = target[0];
        double yRot = target[1];
        double zRot = target[2];

        tweenDelta = (1.0 / (endTime - startTime)) * (1.0 / fps) * rotVec;
    }
}

bool Tween::frameIsAffected(int frameNumber) {
    return (frameNumber >= startTime * fps) && (frameNumber <= endTime * fps);
}

Vector3D Tween::getTweenDelta(int frameNumber) {
    int startFrame = startTime * fps;
    int endFrame = endTime * fps;

    if (frameNumber < startFrame) {
        return Vector3D();
    } else if (frameNumber < endFrame) {
        return (frameNumber - startFrame) * tweenDelta;
    } else {
        return (endFrame - startFrame) * tweenDelta;
    }
}