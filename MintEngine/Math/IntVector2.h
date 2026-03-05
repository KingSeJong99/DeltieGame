#ifndef MINT_ENGINE_MATH_INT_VECTOR2_H_
#define MINT_ENGINE_MATH_INT_VECTOR2_H_

#include <Windows.h>
#include <string>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 격자 구조를 다루기 위한 정수형 좌표 클래스
 */
class MINT_API IntVector2 {
 public:
  /**
   * @brief IntVector2 기본 생성자 (0, 0으로 초기화)
   */
  IntVector2() : x(0), y(0) {}

  /**
   * @brief 지정된 좌표로 IntVector2 생성
   * @param x X축 정수 좌표
   * @param y Y축 정수 좌표
   */
  IntVector2(int x, int y) : x(x), y(y) {}

  ~IntVector2() = default;

  /**
   * @brief 벡터의 정보를 문자열로 반환
   * @return "(x, y)" 형식의 문자열
   */
  std::string ToString() const;

  // 산술 연산자 오버로딩
  IntVector2 operator+(const IntVector2& other) const {
    return IntVector2(x + other.x, y + other.y);
  }
  IntVector2 operator-(const IntVector2& other) const {
    return IntVector2(x - other.x, y - other.y);
  }

  // 비교 연산자 오버로딩
  bool operator==(const IntVector2& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const IntVector2& other) const {
    return !(*this == other);
  }

  /**
   * @brief Windows API의 COORD 구조체로 형변환
   */
  operator COORD() const {
    return {static_cast<SHORT>(x), static_cast<SHORT>(y)};
  }

  // 정적 상수 벡터
  static const IntVector2 kZero;
  static const IntVector2 kOne;
  static const IntVector2 kUp;
  static const IntVector2 kDown;
  static const IntVector2 kLeft;
  static const IntVector2 kRight;

  int x;  ///< X축 정수 좌표
  int y;  ///< Y축 정수 좌표
};

}  // namespace mint

#endif  // MINT_ENGINE_MATH_INT_VECTOR2_H_
