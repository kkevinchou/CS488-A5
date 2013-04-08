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
        x++; // removes warning

        Colour startColour = Colour(0, convColour(188), convColour(209));
        Colour endColour = Colour(convColour(254), convColour(249), convColour(240));

        double sR = startColour.R();
        double sB = startColour.G();
        double sG = startColour.B();

        double eR = endColour.R();
        double eB = endColour.G();
        double eG = endColour.B();

        return Colour(sR + ((eR - sR) * (y / height)),
            sG + ((eG - sG) * (y / height)),
            sB + ((eB - sB) * (y / height)));
    }

private:
    double width;
    double height;

    double convColour(double v) const {
        return v / 255;
    }
};

#endif