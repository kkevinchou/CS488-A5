#include "texturemanager.hpp"

void TextureManager::load(const string &textureFile) {
    Image img;

    if (img.loadPng(textureFile)) {
        cerr << "TEXTURE LOAD SUCCESS!" << endl;
        imageMap[textureFile] = img;
    } else {
        cerr << "TEXTURE LOAD FAILED!" << endl;
    }
}

Colour TextureManager::getTextureColour(const string &textureFile, const Point2D &textureCoordinates) {
    Image img = imageMap[textureFile];

    int u = (int)floor(textureCoordinates[0] * (img.width() - 1));
    int v = (int)floor(textureCoordinates[1] * (img.height() - 1));

    // cerr << "U " << textureCoordinates[0] << endl;
    // cerr << "V " << textureCoordinates[1] << endl;

    Colour colour(img(u, v, 0), img(u, v, 1), img(u, v, 2));

    return colour;
}

TextureManager::TextureManager() {

}