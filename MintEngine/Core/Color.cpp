#include "MintEngine/Core/Color.h"

namespace mint {

Color RgbToLegacyColor(int r, int g, int b) {
  // 각 채널의 밝기를 기준으로 색상 비트를 결정 (임계값 128)
  bool has_red = (r > 128);
  bool has_green = (g > 128);
  bool has_blue = (b > 128);

  // 전체적으로 밝은지 어두운지를 판단 (평균 임계값 128)
  bool is_bright = (r + g + b) > (128 * 3);

  WORD color_code = 0;

  if (has_red) color_code |= FOREGROUND_RED;
  if (has_green) color_code |= FOREGROUND_GREEN;
  if (has_blue) color_code |= FOREGROUND_BLUE;

  // 어떤 색상도 지배적이지 않은 경우 (매우 어둡거나 회색조)
  if (color_code == 0) {
    if (is_bright) return Color::kGray;
    return Color::kBlack;
  }

  // 주 색상이 결정된 상태에서 전반적인 밝기를 고려하여 강조(INTENSITY) 추가
  if (is_bright) {
    color_code |= FOREGROUND_INTENSITY;
  }

  return static_cast<Color>(color_code);
}

}  // namespace mint
