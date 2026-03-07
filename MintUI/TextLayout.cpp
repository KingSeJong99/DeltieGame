#include "MintUI/TextLayout.h"

#include <algorithm>

namespace mint {
namespace ui {

TextLayout::TextLayout(Renderer& renderer) : renderer_(renderer) {}

void TextLayout::DrawBox(int x, int y, int width, int height,
                         const std::wstring& title, Color border_color,
                         Color bg_color, Color title_color, int sorting_order) {
  // 상단 테두리
  renderer_.SetCell(x, y, L'┏', border_color, bg_color, sorting_order);
  for (int i = 1; i < width - 1; ++i) {
    renderer_.SetCell(x + i, y, L'━', border_color, bg_color, sorting_order);
  }
  renderer_.SetCell(x + width - 1, y, L'┓', border_color, bg_color,
                    sorting_order);

  // 제목 출력 (상단 테두리 중앙)
  if (!title.empty()) {
    int title_width = Renderer::CalculateVisualWidth(title);
    int start_x = x + (width - title_width) / 2;
    renderer_.Submit(title, Vector2(static_cast<float>(start_x), static_cast<float>(y)),
                     title_color, sorting_order + 1);
  }

  // 측면 테두리 및 배경 채우기
  for (int j = 1; j < height - 1; ++j) {
    renderer_.SetCell(x, y + j, L'┃', border_color, bg_color, sorting_order);
    for (int i = 1; i < width - 1; ++i) {
      renderer_.SetCell(x + i, y + j, L' ', border_color, bg_color,
                        sorting_order);
    }
    renderer_.SetCell(x + width - 1, y + j, L'┃', border_color, bg_color,
                      sorting_order);
  }

  // 하단 테두리
  renderer_.SetCell(x, y + height - 1, L'┗', border_color, bg_color,
                    sorting_order);
  for (int i = 1; i < width - 1; ++i) {
    renderer_.SetCell(x + i, y + height - 1, L'━', border_color, bg_color,
                      sorting_order);
  }
  renderer_.SetCell(x + width - 1, y + height - 1, L'┛', border_color,
                    bg_color, sorting_order);
}

void TextLayout::DrawTextAligned(int x, int y, int width,
                                 const std::wstring& text, Alignment align,
                                 Color fg_color, Color bg_color,
                                 int sorting_order) {
  int text_visual_width = Renderer::CalculateVisualWidth(text);
  int draw_x = x;

  switch (align) {
    case Alignment::Center:
      draw_x = x + (width - text_visual_width) / 2;
      break;
    case Alignment::Right:
      draw_x = x + width - text_visual_width;
      break;
    case Alignment::Left:
    default:
      draw_x = x;
      break;
  }

  renderer_.Submit(text, Vector2(static_cast<float>(draw_x), static_cast<float>(y)),
                   fg_color, sorting_order);
}

void TextLayout::DrawProgressBar(int x, int y, int width, int height,
                                 float percentage, wchar_t fill_char,
                                 Color fg_color, Color bg_color,
                                 int sorting_order) {
  percentage = std::clamp(percentage, 0.0f, 1.0f);
  int fill_width = static_cast<int>(width * percentage);

  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      Color current_color = (i < fill_width) ? fg_color : bg_color;
      renderer_.SetCell(x + i, y + j, fill_char, current_color, Color::kBlack,
                        sorting_order);
    }
  }
}

}  // namespace ui
}  // namespace mint
