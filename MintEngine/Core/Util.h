#ifndef MINT_ENGINE_CORE_UTIL_H_
#define MINT_ENGINE_CORE_UTIL_H_

#include <Windows.h>
#include <string>
#include <vector>
#include <sstream>

#include "MintEngine/Core/Color.h"
#include "MintEngine/Math/Vector2.h"

namespace mint {

/**
 * @brief 유틸리티 함수들을 모아둔 네임스페이스
 */
namespace util {

/**
 * @brief 문자열을 특정 구분자로 분리하여 벡터로 반환한다.
 * @param str 원본 문자열.
 * @param delimiter 구분자 문자.
 * @return 분리된 문자열들의 벡터.
 */
inline std::vector<std::wstring> Split(const std::wstring& str, wchar_t delimiter) {
  std::vector<std::wstring> tokens;
  std::wstringstream ss(str);
  std::wstring item;
  while (std::getline(ss, item, delimiter)) {
    tokens.push_back(item);
  }
  return tokens;
}

/**
 * @brief 문자열의 앞뒤 공백을 제거.
 * @param str 원본 문자열.
 * @return 공백이 제거된 문자열.
 */
inline std::wstring Trim(const std::wstring& str) {
  if (str.empty()) return L"";
  const std::wstring whitespace = L" \n\r\t\f\v";
  const size_t start = str.find_first_not_of(whitespace);
  if (start == std::wstring::npos) return L"";
  const size_t end = str.find_last_not_of(whitespace);
  return str.substr(start, end - start + 1);
}

/**
 * @brief UTF-8 멀티바이트 문자열을 WideString(wstring)으로 변환
 * @param str UTF-8 문자열
 * @return 변환된 wstring
 */
inline std::wstring UTF8ToWide(const std::string& str) {
  if (str.empty()) return L"";
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
  std::wstring wstr_to(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr_to[0], size_needed);
  return wstr_to;
}

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
