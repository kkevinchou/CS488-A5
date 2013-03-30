#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

#include <string>
#include <map>
#include <iostream>

#include "image.hpp"
#include "algebra.hpp"

using namespace std;

class TextureManager {
public:
    void load(const string &textureFile);
    Colour getTextureColour(const string &textureFile, const Point2D &textureCoordinates);
    TextureManager();
private:
    map<string, Image> imageMap;
};

#endif