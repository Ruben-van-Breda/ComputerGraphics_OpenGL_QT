/*
 *
 * PPMImage.cpp
 * Simple 8-bit PPM based image class
 *
 */

#define MAX_IMAGE_DIMENSION 4096

#include "PPMImage.h"
#include <QDebug>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <QOpenGLFunctions_3_0>

PPMImage::PPMImage() : _data(nullptr), _width(0), _height(0) {}

PPMImage::~PPMImage() {
    if (_data != nullptr)
        delete[] _data;
}

// Resizes array - allows us to delay construction
bool PPMImage::resize(Index width, Index height) {
    if ((width > MAX_IMAGE_DIMENSION) || (height > MAX_IMAGE_DIMENSION)) {
        qDebug() << "Cannot handle image of size " << width << "x" << height;
        return false;
    }

    // if same size, do nothing
    if ((width == _width) && (height == _height)) {
        return false;
    }

    // if there is existing data in the image this
    // operation should be taken
    if (_data != nullptr) {
        Byte *temp = new Byte[width * height * 4]; // allocate a new block of memory
        std::memset(temp, 255, width * height * 4); // make background white

        Index w = (_width < width) ? _width : width;
        Index h = (_height < height) ? _height : height;

        // copy data to new block
        for (Index row = 0; row < h; row++) {
            for (Index col = 0; col < w; col++) {
                for (Byte byte = 0; byte < 4; byte++) {
                    temp[byte + (col * 4) + (row * 4 * width)] =
                            _data[byte + (col * 4) + (row * 4 * _width)];
                }
            }
        }

        delete[] _data; // delete old data
        _data = temp;   // copy pointer
    } else {          // otherwise allocate new memory
        _data = new Byte[width * height * 4];
        std::memset(_data, 255, width * height * 4); // make background white
    }
    _height = height;
    _width = width; // assign the new dimensions
    return true;
}

void PPMImage::setPixel(Index row, Index col, Byte red, Byte green, Byte blue,
                        Byte alpha) {

    if ((row >= _height) || (col >= _width))
        return;

    _data[(col * 4) + (row * 4 * _width)] = red;
    _data[1 + (col * 4) + (row * 4 * _width)] = green;
    _data[2 + (col * 4) + (row * 4 * _width)] = blue;
    _data[3 + (col * 4) + (row * 4 * _width)] = alpha;
}

void PPMImage::setPixel(Index row, Index col, RGBValue colour, Byte alpha) {
    this->setPixel(row, col, colour.red, colour.green, colour.blue, alpha);
}

// Read PPM image from file
bool PPMImage::readFile(std::string fileName) {

    char buffer[1024]; // buffer for reading
    std::ifstream inFile(fileName.c_str(), std::ifstream::in);

    if (inFile.is_open()) {

        inFile.getline(buffer, 1000, '\n'); // get the first line form the file

        // read in magic number
        if (std::strstr(buffer, "P3") != buffer) {
            qDebug() << "PPM file " << fileName.c_str()
                     << " did not start with PPM code (P3)";
            return false;
        }

        inFile.getline(buffer, 1000, '\n'); // get next line

        // read in and discard all comment lines
        while (buffer[0] == '#') {
            inFile.getline(buffer, 1000, '\n');
        }

        Index newWidth, newHeight;
        int nScanned = std::sscanf(buffer, "%d %d\n", &newWidth, &newHeight);

        if (nScanned != 2) {
            qDebug() << "Failed to read width and height.";
            return false;
        } /* failure */

        int maxColour;       // temporary variables
        inFile >> maxColour; // extract the dimensions

        if (maxColour != 255) {
            qDebug() << "PPM file " << fileName.c_str()
                     << " did not specify 255 as the maximum colour value";
            return false;
        }
        /* check for stupid sizes */
        if ((newWidth < 1) || (newWidth > MAX_IMAGE_DIMENSION) || (newHeight < 1) ||
                (newHeight > MAX_IMAGE_DIMENSION)) {
            qDebug() << "PPM file dimensions were outside range of 1 - "
                     << MAX_IMAGE_DIMENSION;
        }

        _height = newHeight;
        _width = newWidth;

        _data = new Byte[_width * _height * 4];

        for (Index row = 0; row < _height; row++) {
            for (Index col = 0; col < _width; col++) {
                Index rrow = (_height - 1 - row);
                Byte red, green, blue;
                inFile >> red >> green >> blue;
                _data[(col * 4) + (rrow * 4 * _width)] = red;
                _data[1 + (col * 4) + (rrow * 4 * _width)] = green;
                _data[2 + (col * 4) + (rrow * 4 * _width)] = blue;
                _data[3 + (col * 4) + (rrow * 4 * _width)] = 255;
            }
        }

        inFile.close();
        return true;
    } else {
        qDebug() << "Failed to open : " << fileName.c_str();
        return false;
    }
}

// Write PPM image to file
bool PPMImage::writeFile(std::string fileName) {

    std::ofstream outFile(fileName.c_str(), std::ofstream::out);
    if (outFile.is_open()) {
        outFile << "P3\n";
        outFile << "# Created for COMP30020\n";
        outFile << _width << " " << _height << "\n";
        outFile << 255 << "\n";
        for (Index row = 0; row < _height; row++) {
            for (Index col = 0; col < _width; col++) {
                Index rrow = (_height - 1 - row); // the pixels reversed scan-line order
                for (Byte byte = 0; byte < 3; byte++) {
                    outFile << static_cast<int>(
                                   _data[byte + (col * 4) + (rrow * 4 * _width)])
                            << " ";
                }
                outFile << "\n";
            }
        }
        outFile.close();
        return true;
    } else {
        qDebug() << "Failed to write : " << fileName.c_str();
        return false;
    }
}

// Clear PPM image
void PPMImage::clear() { std::memset(_data, 255, _width * _height * 4); }

// Render PPM image in OpenGL
void PPMImage::draw() {
    // set up image parameters
    glRasterPos2i(0, 0);
    glPixelZoom(zoom, zoom);
    // draw the image
    glDrawPixels(static_cast<int>(_width), static_cast<int>(_height), GL_RGBA, GL_UNSIGNED_BYTE, _data);
}
