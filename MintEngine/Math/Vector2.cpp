#include "MintEngine/Math/Vector2.h"

#include <cstdio>

namespace mint {

const Vector2 Vector2::kZero(0.0f, 0.0f);
const Vector2 Vector2::kOne(1.0f, 1.0f);
const Vector2 Vector2::kUp(0.0f, 1.0f);
const Vector2 Vector2::kRight(1.0f, 0.0f);

Vector2::Vector2() : x(0.0f), y(0.0f) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

std::string Vector2::ToString() const {
  char buffer[64];
  sprintf_s(buffer, "(%.2f, %.2f)", x, y);
  return std::string(buffer);
}

Vector2 Vector2::operator+(const Vector2& other) const {
  return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
  return Vector2(x - other.x, y - other.y);
}

bool Vector2::operator==(const Vector2& other) const {
  return (x == other.x) && (y == other.y);
}

bool Vector2::operator!=(const Vector2& other) const {
  return !(*this == other);
}

Vector2::operator COORD() const {
  return {static_cast<short>(x), static_cast<short>(y)};
}

}  // namespace mint
