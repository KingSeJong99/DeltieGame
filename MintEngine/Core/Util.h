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

/**
 * @brief 콘솔 커서의 위치를 설정
 * @param position 이동할 좌표 (Vector2)
 */
inline void SetConsolePosition(const Vector2& position) {
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                           static_cast<COORD>(position));
}

/**
 * @brief 콘솔 출력 텍스트의 색상을 설정
 * @param color 적용할 색상 (Color 열거형)
 */
inline void SetConsoleTextColor(Color color) {
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          static_cast<unsigned short>(color));
}

/**
 * @brief 콘솔 커서를 보이지 않게 설정
 */
inline void TurnOffCursor() {
  CONSOLE_CURSOR_INFO info = {};
  GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
  info.bVisible = FALSE;
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

/**
 * @brief 콘솔 커서를 화면에 표시되도록 설정
 */
inline void TurnOnCursor() {
  CONSOLE_CURSOR_INFO info = {};
  GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
  info.bVisible = TRUE;
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

}  // namespace util

/**
 * @brief 할당된 동적 메모리를 해제하고 포인터를 초기화 (단일 객체용)
 * @tparam T 해제할 객체의 타입
 * @param t 해제할 객체의 포인터 참조
 */
template <typename T>
void SafeDelete(T*& t) {
  if (t) {
    delete t;
    t = nullptr;
  }
}

/**
 * @brief 할당된 동적 메모리 배열을 해제하고 포인터를 초기화 (배열용)
 * @tparam T 해제할 배열 요소의 타입
 * @param t 해제할 배열의 포인터 참조
 */
template <typename T>
void SafeDeleteArray(T*& t) {
  if (t) {
    delete[] t;
    t = nullptr;
  }
}

}  // namespace mint

#endif  // MINT_ENGINE_CORE_UTIL_H_
