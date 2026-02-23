#ifndef MINT_ENGINE_CORE_COLOR_H_
#define MINT_ENGINE_CORE_COLOR_H_

#include <Windows.h>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 콘솔 텍스트 및 배경색을 정의하는 열거형
 * Windows API의 FOREGROUND_* 매크로와 호환됨
 */
enum class MINT_API Color : WORD {
  kBlack = 0,
  kBlue = FOREGROUND_BLUE,
  kGreen = FOREGROUND_GREEN,
  kCyan = FOREGROUND_BLUE | FOREGROUND_GREEN,
  kRed = FOREGROUND_RED,
  kMagenta = FOREGROUND_RED | FOREGROUND_BLUE,
  kYellow = FOREGROUND_RED | FOREGROUND_GREEN,
  kWhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
  kGray = FOREGROUND_INTENSITY,
  kBrightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
  kBrightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
  kBrightCyan = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
  kBrightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
  kBrightMagenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
  kBrightYellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
  kBrightWhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE |
                 FOREGROUND_INTENSITY,
};

/**
 * @brief RGB 값을 콘솔에서 사용 가능한 가장 가까운 레거시 색상으로 변환
 */
Color RgbToLegacyColor(int r, int g, int b);

}  // namespace mint

#endif  // MINT_ENGINE_CORE_COLOR_H_
