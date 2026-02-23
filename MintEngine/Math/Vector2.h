#ifndef MINT_ENGINE_MATH_VECTOR2_H_
#define MINT_ENGINE_MATH_VECTOR2_H_

#include <Windows.h>
#include <string>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 2차원 벡터를 표현하는 구조체
 */
class MINT_API Vector2 {
 public:
  Vector2();
  Vector2(float x, float y);
  ~Vector2() = default;

  // 벡터 정보를 문자열로 반환
  std::string ToString() const;

  Vector2 operator+(const Vector2& other) const;
  Vector2 operator-(const Vector2& other) const;

  bool operator==(const Vector2& other) const;
  bool operator!=(const Vector2& other) const;

  // Windows COORD 구조체로의 형변환
  operator COORD() const;

  // 정적 상수 벡터
  static const Vector2 kZero;
  static const Vector2 kOne;
  static const Vector2 kUp;
  static const Vector2 kRight;

  float x = 0.0f;
  float y = 0.0f;
};

}  // namespace mint

#endif  // MINT_ENGINE_MATH_VECTOR2_H_
