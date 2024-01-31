/*
 *
 * RGBValue.cpp
 * Simple class to encapsulate RGB colour
 *
 */

#include <iostream>

#include "RGBValue.h"

// default constructor
RGBValue::RGBValue() { red = green = blue = 0; }

// value constructor
RGBValue::RGBValue(Byte Red, Byte Green, Byte Blue) {
    red = Red;
    green = Green;
    blue = Blue;
}

Byte &RGBValue::operator[](int i) {
    switch (i) {
    case 0:
        return red;
    case 1:
        return green;
    case 2:
        return blue;
    default:
        std::cout << "RGBValue::operator [](int i) - Operator out of bounds: " << i
                  << std::endl;
        return red;
    }
}

// Unary Minus
const RGBValue operator-(const RGBValue &right) {
    return RGBValue(-right.red, -right.green, -right.blue);
}

// Addition
const RGBValue operator+(const RGBValue &left, const RGBValue &right) {
    return RGBValue(left.red + right.red, left.green + right.green,
                    left.blue + right.blue);
}

// Subtraction
const RGBValue operator-(const RGBValue &left, const RGBValue &right) {
    return RGBValue(left.red - right.red, left.green - right.green,
                    left.blue - right.blue);
}

// Scalar Multiplication
const RGBValue operator*(float scalar, const RGBValue &colour) {
    return RGBValue(static_cast<unsigned char>(scalar * colour.red),
                    static_cast<unsigned char>(scalar * colour.green),
                    static_cast<unsigned char>(scalar * colour.blue));
}

const RGBValue operator*(const RGBValue &colour, float scalar) {
    return RGBValue(static_cast<unsigned char>(scalar * colour.red),
                    static_cast<unsigned char>(scalar * colour.green),
                    static_cast<unsigned char>(scalar * colour.blue));
}
