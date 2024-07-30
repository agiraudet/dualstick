#include "Vector.hpp"
#include <cmath>

Vector::Vector(void) : x(0), y(0) {}

Vector::Vector(Vector const &src) : x(src.x), y(src.y) {}

Vector::Vector(float const x, float const y) : x(x), y(y) {}

Vector::~Vector(void) {}

void Vector::setX(float const x) { this->x = x; }

void Vector::setY(float const y) { this->y = y; }

void Vector::setXY(float const x, float const y) {
  this->x = x;
  this->y = y;
}

void Vector::capIntensity(float const maxIntensity) {
  float inten = intensity();
  if (inten > maxIntensity) {
    float factor = maxIntensity / inten;
    x /= factor;
    y /= factor;
  }
}

Vector Vector::normalize(void) const {
  float mag = magnitude();
  if (mag == 0) {
    return Vector(0, 0);
  }
  return Vector(x / mag, y / mag);
}

float Vector::getX(void) const { return x; }

float Vector::getY(void) const { return y; }

float Vector::intensity(void) const { return x * x + y * y; }

float Vector::magnitude(void) const { return std::sqrt(x * x + y * y); }

Vector &Vector::operator=(Vector const &other) {
  if (this != &other) {
    x = other.x;
    y = other.y;
  }
  return *this;
}

// Comparison operators
bool Vector::operator>(Vector const &other) const {
  return magnitude() > other.magnitude();
}

bool Vector::operator>=(Vector const &other) const {
  return magnitude() >= other.magnitude();
}

bool Vector::operator<(Vector const &other) const {
  return magnitude() < other.magnitude();
}

bool Vector::operator<=(Vector const &other) const {
  return magnitude() <= other.magnitude();
}

bool Vector::operator==(Vector const &other) const {
  return x == other.x && y == other.y;
}

bool Vector::operator!=(Vector const &other) const { return !(*this == other); }

// Arithmetic operators
Vector Vector::operator+(Vector const &other) const {
  return Vector(x + other.x, y + other.y);
}

Vector Vector::operator-(Vector const &other) const {
  return Vector(x - other.x, y - other.y);
}

Vector Vector::operator*(Vector const &other) const {
  return Vector(x * other.x, y * other.y);
}

Vector Vector::operator/(Vector const &other) const {
  return Vector(x / other.x, y / other.y);
}

// Compound assignment operators
Vector &Vector::operator+=(Vector const &other) {
  x += other.x;
  y += other.y;
  return *this;
}

Vector &Vector::operator-=(Vector const &other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

Vector &Vector::operator*=(Vector const &other) {
  x *= other.x;
  y *= other.y;
  return *this;
}

Vector &Vector::operator/=(Vector const &other) {
  x /= other.x;
  y /= other.y;
  return *this;
}

// Min and max functions
Vector const &Vector::min(Vector const &a, Vector const &b) {
  return (a.magnitude() < b.magnitude()) ? a : b;
}

Vector &Vector::min(Vector &a, Vector &b) {
  return (a.magnitude() < b.magnitude()) ? a : b;
}

Vector const &Vector::max(Vector const &a, Vector const &b) {
  return (a.magnitude() > b.magnitude()) ? a : b;
}

Vector &Vector::max(Vector &a, Vector &b) {
  return (a.magnitude() > b.magnitude()) ? a : b;
}

// Output stream operator
std::ostream &operator<<(std::ostream &o, Vector const &v) {
  o << "Vector(" << v.getX() << ", " << v.getY() << ")";
  return o;
}
