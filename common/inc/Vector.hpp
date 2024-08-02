#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>

struct Vector {
  float x;
  float y;

  Vector(void);
  Vector(Vector const &src);
  Vector(float const x, float const y);
  ~Vector(void);

  void setX(float const x);
  void setY(float const y);
  void setXY(float const x, float const y);
  void capIntensity(float const maxIntensity);
  Vector normalize(void) const;

  float getX(void) const;
  float getY(void) const;
  float intensity(void) const;
  float magnitude(void) const;
  float distance(Vector const &other) const;
  float distanceSq(Vector const &other) const;

  Vector &operator=(Vector const &other);
  bool operator>(Vector const &other) const;
  bool operator>=(Vector const &other) const;
  bool operator<(Vector const &other) const;
  bool operator<=(Vector const &other) const;
  bool operator==(Vector const &other) const;
  bool operator!=(Vector const &other) const;
  Vector operator+(Vector const &other) const;
  Vector operator-(Vector const &other) const;
  Vector operator*(Vector const &other) const;
  Vector operator/(Vector const &other) const;
  Vector operator+(float scalar) const;
  Vector operator-(float scalar) const;
  Vector operator*(float scalar) const;
  Vector operator/(float scalar) const;
  Vector &operator+=(Vector const &other);
  Vector &operator-=(Vector const &other);
  Vector &operator*=(Vector const &other);
  Vector &operator/=(Vector const &other);
  static Vector const &min(Vector const &a, Vector const &b);
  static Vector &min(Vector &a, Vector &b);
  static Vector const &max(Vector const &a, Vector const &b);
  static Vector &max(Vector &a, Vector &b);
};

std::ostream &operator<<(std::ostream &o, Vector const &other);

#endif
