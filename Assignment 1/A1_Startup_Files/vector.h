#pragma once

#include "point.h"
#include <iostream>

class Vector {

public:             // makes life easier
    float dx, dy, dz; // the coordinates

public:
    Vector();                                   // default constructor
    Vector(float v_dx, float v_dy, float v_dz); // initializing constructor

    float operator[](int i) const;
    float norm() const;
    float dot(const Vector &v) const;

    const Vector normalised() const;
    const Vector cross(const Vector &v) const;
};

// C++ print routine
std::ostream &operator<<(std::ostream &ostr, Vector v);

// Vector/Point Operations

// Unary Minuse
const Vector operator-(const Vector &v);

// Addition
const Point operator+(const Point &p, const Vector &v);
const Vector operator+(const Vector &v, const Vector &w);

// Subtraction
const Vector operator-(const Point &tail, const Point &head);
const Point operator-(const Point &p, const Vector &v);
const Vector operator-(const Vector &v, const Vector &w);

// Scalar Multiplication
const Vector operator*(float s, const Vector &v);
const Vector operator*(const Vector &v, float s);
