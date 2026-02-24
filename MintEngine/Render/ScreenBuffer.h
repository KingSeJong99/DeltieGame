#ifndef MINT_ENGINE_RENDER_SCREEN_BUFFER_H_
#define MINT_ENGINE_RENDER_SCREEN_BUFFER_H_

#include <Windows.h>

#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 콘솔 출력 핸들을 관리하여 화면 버퍼링을 처리하는 클래스
 */
class MINT_API ScreenBuffer {
 public:
  /**
   * @brief 스크린 버퍼 생성자
   * @param screen_size 버퍼의 가로, 세로 크기
   */
  explicit ScreenBuffer(const Vector2& screen_size);

  /**
   * @brief 스크린 버퍼 소멸자 (콘솔 핸들을 닫음)
   */
  ~ScreenBuffer();

  // 복사 및 이동 제한 (핸들 관리 클래스의 안전성을 위함)
  ScreenBuffer(const ScreenBuffer&) = delete;
  ScreenBuffer& operator=(const ScreenBuffer&) = delete;

  /**
   * @brief 콘솔 버퍼의 모든 내용을 빈 문자로 채움
   */
  void Clear();

  /**
   * @brief 콘솔에 CHAR_INFO 배열 데이터를 출력
   * @param char_info 출력할 문자 및 색상 정보가 담긴 배열
   */
  void Draw(const CHAR_INFO* char_info);

  /**
   * @brief 관리 중인 콘솔 버퍼 핸들을 반환
   * @return 콘솔 출력 핸들 (HANDLE)
   */
  inline HANDLE buffer() const { return buffer_; }

 private:
  HANDLE buffer_ = nullptr;  ///< WinAPI 콘솔 출력 핸들
  Vector2 screen_size_;      ///< 버퍼의 크기
};

}  // namespace mint

#endif  // MINT_ENGINE_RENDER_SCREEN_BUFFER_H_
