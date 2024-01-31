#include "point.h"
#include <iostream>

using namespace std;

// default constructor
Point::Point() { x = y = z = 0.0f; }

// initializing constructor
Point::Point(float p_x, float p_y, float p_z) {
    x = p_x;
    y = p_y;
    z = p_z;
}

float Point::operator[](int i) const {
    switch (i) {
    case 0:
        return x; // x is equivalent to index 0
    case 1:
        return y; // y is equivalent to index 1
    case 2:
        return z; // z is equivalent to index 2
    default:
        std::cout << "Error: . . . .";
        return x; // we have to return something
    }
}

// C++ print routine
std::ostream &operator<<(std::ostream &ostr, Point p) {
    ostr << "[" << p.x << ", " << p.y << ", " << p.z << "]";
    return ostr;
}

// Scalar Multiplication
const Point operator*(float s, const Point &p) {
    return Point((p[0] * s), (p[1] * s), (p[2] * s));
}

const Point operator*(const Point &p, float s) {
    return Point((p[0] * s), (p[1] * s), (p[2] * s));
}
