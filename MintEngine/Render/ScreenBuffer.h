#ifndef MINT_ENGINE_RENDER_SCREEN_BUFFER_H_
#define MINT_ENGINE_RENDER_SCREEN_BUFFER_H_

#include <Windows.h>

#include "MintEngine/Math/Vector2.h"

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
   * @brief 스크린 버퍼 소멸자
   */
  ~ScreenBuffer();

  /**
   * @brief 콘솔 버퍼의 내용을 지우는 함수
   */
  void Clear();

  /**
   * @brief 콘솔에 CHAR_INFO 배열 데이터를 그리는 함수
   * @param char_info 콘솔에 출력할 문자 및 색상 정보 배열
   */
  void Draw(CHAR_INFO* char_info);

  /**
   * @brief 콘솔 버퍼 핸들을 반환하는 Getter
   * @return 현재 관리 중인 콘솔 출력 핸들 (HANDLE)
   */
  inline HANDLE GetBuffer() const { return buffer_; }

 private: 
  HANDLE buffer_ = nullptr;  ///< 콘솔 출력 핸들
  Vector2 screen_size_;      ///< 화면 크기
};

}  // namespace mint

#endif  // MINT_ENGINE_RENDER_SCREEN_BUFFER_H_
