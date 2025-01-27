/*
 *
 * RGBValue.h
 * Simple class to encapsulate RGB colour
 *
 */

#pragma once

typedef unsigned char Byte;

class RGBValue {
public:
    // just a container for the three components
    Byte red, green, blue;

    // constructor / destructor
    RGBValue();
    RGBValue(Byte Red, Byte Green, Byte Blue);
    Byte &operator[](int i);
};

// pixel operations
const RGBValue operator-(const RGBValue &right);
const RGBValue operator+(const RGBValue &left, const RGBValue &right);
const RGBValue operator-(const RGBValue &left, const RGBValue &right);
const RGBValue operator*(float scalar, const RGBValue &colour);
const RGBValue operator*(const RGBValue &colour, float scalar);
