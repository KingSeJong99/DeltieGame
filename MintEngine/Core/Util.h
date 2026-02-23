#ifndef MINT_ENGINE_CORE_UTIL_H_
#define MINT_ENGINE_CORE_UTIL_H_

#include <Windows.h>

#include "MintEngine/Core/Color.h"
#include "MintEngine/Math/Vector2.h"

namespace mint {

/**
 * @brief 유틸리티 함수들을 모아둔 네임스페이스
 */
namespace util {

// 콘솔 커서 위치를 설정하는 함수
inline void SetConsolePosition(const Vector2& position) {
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                           static_cast<COORD>(position));
}

// 콘솔 텍스트 색상을 설정하는 함수
inline void SetConsoleTextColor(Color color) {
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          static_cast<unsigned short>(color));
}

// 콘솔 커서를 보이지 않게 설정
inline void TurnOffCursor() {
  CONSOLE_CURSOR_INFO info = {};
  GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
  info.bVisible = FALSE;
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

// 콘솔 커서를 보이게 설정
inline void TurnOnCursor() {
  CONSOLE_CURSOR_INFO info = {};
  GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
  info.bVisible = TRUE;
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

}  // namespace util

// 메모리 해제 헬퍼 (단일 객체)
template <typename T>
void SafeDelete(T*& t) {
  if (t) {
    delete t;
    t = nullptr;
  }
}

// 메모리 해제 헬퍼 (배열)
template <typename T>
void SafeDeleteArray(T*& t) {
  if (t) {
    delete[] t;
    t = nullptr;
  }
}

}  // namespace mint

#endif  // MINT_ENGINE_CORE_UTIL_H_
