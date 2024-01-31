#include "vector.h"
#include <math.h>

Vector::Vector() { dx = dy = dz = 0.0f; }

Vector::Vector(float v_dx, float v_dy, float v_dz) {
    dx = v_dx;
    dy = v_dy;
    dz = v_dz;
}

float Vector::operator[](int i) const {
    switch (i) {
    case 0:
        return dx; // dx is equivalent to index 0
    case 1:
        return dy; // dy is equivalent to index 1
    case 2:
        return dz; // dz is equivalent to index 2
    default:
        std::cout << "Error: . . . .";
        return dx;
    }
}

float Vector::norm() const { return sqrt(dx * dx + dy * dy + dz * dz); }

const Vector Vector::normalised() const {
    float norm = this->norm();
    return Vector(dx / norm, dy / norm, dz / norm);
}

float Vector::dot(const Vector &v) const {
    return (dx * v.dx + dy * v.dy + dz * v.dz);
}

const Vector Vector::cross(const Vector &v) const {
    return Vector((dy * v.dz - dz * v.dy), (dz * v.dx - dx * v.dz),
                  (dx * v.dy - dy * v.dx));
}

// C++ print routine
std::ostream &operator<<(std::ostream &ostr, Vector v) {
    ostr << "[" << v.dx << ", " << v.dy << ", " << v.dz << "]";
    return ostr;
}

// Unary Minuse
const Vector operator-(const Vector &v) { return -1.0f * v; }

// Addition
const Point operator+(const Point &p, const Vector &v) {
    return Point((p[0] + v[0]), (p[1] + v[1]), (p[2] + v[2]));
}

const Vector operator+(const Vector &v, const Vector &w) {
    return Vector((v[0] + w[0]), (v[1] + w[1]), (v[2] + w[2]));
}
// Subtraction
const Point operator-(Point p, Vector v) {
    return Point((p[0] - v[0]), (p[1] - v[1]), (p[2] - v[2]));
}

const Vector operator-(Vector v, Vector w) {
    return Vector((v[0] - w[0]), (v[1] - w[1]), (v[2] - w[2]));
}

const Vector operator-(const Point &tail, const Point &head) {
    return Vector((tail[0] - head[0]), (tail[1] - head[1]), (tail[2] - head[2]));
}

// Scalar Multiplication
const Vector operator*(float s, const Vector &v) {
    return Vector((v[0] * s), (v[1] * s), (v[2] * s));
}

const Vector operator*(const Vector &v, float s) {
    return Vector((v[0] * s), (v[1] * s), (v[2] * s));
}
