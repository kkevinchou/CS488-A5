#ifndef TWEEN_HPP
#define TWEEN_HPP

#include "algebra.hpp"

class Tween {
public:
    enum Type {
        TRANSLATE,
        ROTATE,
        JUMP
    };

    Tween();
    Tween(Type type, double startTime, double endTime, Point3D target);
    bool frameIsAffected(int frameNumber);
    void init();
    Vector3D getTweenDelta(int frameNumber);

    Type type;
    double startTime;
    double endTime;
    Point3D target;

private:
    Vector3D tweenDelta;
};

#endif