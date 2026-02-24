#ifndef MINT_ENGINE_MATH_VECTOR2_H_
#define MINT_ENGINE_MATH_VECTOR2_H_

#include <Windows.h>
#include <string>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 2차원 공간의 좌표 및 방향을 표현하는 클래스
 */
class MINT_API Vector2 {
 public:
  /**
   * @brief Vector2 기본 생성자 (원점으로 초기화)
   */
  Vector2();

  /**
   * @brief 지정된 좌표로 Vector2 생성
   * @param x X축 좌표값
   * @param y Y축 좌표값
   */
  Vector2(float x, float y);

  /**
   * @brief Vector2 소멸자
   */
  ~Vector2() = default;
  
  /**
   * @brief 벡터의 정보를 문자열로 반환
   * @return "(x, y)" 형식의 문자열
   */
  std::string ToString() const;

  // 산술 연산자 오버로딩
  Vector2 operator+(const Vector2& other) const;
  Vector2 operator-(const Vector2& other) const;

  // 비교 연산자 오버로딩
  bool operator==(const Vector2& other) const;
  bool operator!=(const Vector2& other) const;

  /**
   * @brief Windows API의 COORD 구조체로 형변환
   * @return 해당 벡터의 정수형 좌표를 담은 COORD 객체
   */
  operator COORD() const;

  // 정적 상수 벡터
  static const Vector2 kZero;   ///< 원점 벡터 (0, 0)
  static const Vector2 kOne;    ///< 단위 벡터 (1, 1)
  static const Vector2 kUp;     ///< 상단 방향 벡터 (0, 1)
  static const Vector2 kRight;  ///< 우측 방향 벡터 (1, 0)

  float x = 0.0f;  ///< X축 좌표
  float y = 0.0f;  ///< Y축 좌표
};

}  // namespace mint

#endif  // MINT_ENGINE_MATH_VECTOR2_H_
