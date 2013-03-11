#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "algebra.hpp"

using namespace std;

class Background {
public:
    Background(double width, double height) : width(width), height(height) {
    }

    void setWidth(double width) {
        this->width = width;
    }

    void setHeight(double height) {
        this->height = height;
    }

    Colour getPixelColour(double x, double y) const {
        return Colour(min((double)(y / height + 0.4), 8.0), 0, min((double)(y / height + 0.4), 8.0));
    }

private:
    double width;
    double height;
};

#endif