#ifndef MINT_UI_TEXT_LAYOUT_H_
#define MINT_UI_TEXT_LAYOUT_H_

#include <string>
#include <vector>

#include "MintEngine/Render/Renderer.h"
#include "MintUI/MintUI_API.h"

namespace mint {
namespace ui {

/**
 * @brief 텍스트 정렬 방식
 */
enum class Alignment { Left, Center, Right };

/**
 * @brief 렌더러를 사용하여 UI 요소(박스, 텍스트 정렬 등)를 그리는 유틸리티
 * 클래스
 */
class MINT_UI_API TextLayout {
 public:
  /**
   * @brief TextLayout 생성자
   * @param renderer 그리기를 수행할 렌더러 인스턴스 (참조)
   */
  explicit TextLayout(Renderer& renderer);

  /**
   * @brief 지정된 위치에 사각형 테두리와 제목을 그리는 함수
   * @param x 시작 X 좌표
   * @param y 시작 Y 좌표
   * @param width 박스의 가로 폭
   * @param height 박스의 세로 높이
   * @param title 박스 상단에 표시할 제목
   * @param border_color 테두리 색상
   * @param bg_color 배경 색상
   * @param title_color 제목 텍스트 색상
   * @param sorting_order 그리기 우선순위
   */
  void DrawBox(int x, int y, int width, int height,
               const std::wstring& title = L"",
               Color border_color = Color::kWhite,
               Color bg_color = Color::kBlack,
               Color title_color = Color::kWhite, int sorting_order = 0);

  /**
   * @brief 지정된 영역 내에서 정렬된 텍스트를 그리는 함수
   * @param x 영역의 시작 X 좌표
   * @param y 영역의 시작 Y 좌표
   * @param width 영역의 가로 폭
   * @param text 출력할 텍스트
   * @param align 정렬 방식 (Left, Center, Right)
   * @param fg_color 텍스트 색상
   * @param bg_color 배경 색상
   * @param sorting_order 그리기 우선순위
   */
  void DrawTextAligned(int x, int y, int width, const std::wstring& text,
                       Alignment align = Alignment::Left,
                       Color fg_color = Color::kWhite,
                       Color bg_color = Color::kBlack, int sorting_order = 0);

  /**
   * @brief 진행 상태를 표시하는 막대(HP바 등)를 그리는 함수
   * @param x 시작 X 좌표
   * @param y 시작 Y 좌표
   * @param width 막대의 전체 가로 폭
   * @param height 막대의 세로 높이
   * @param percentage 진행도 (0.0 ~ 1.0)
   * @param fill_char 채워질 문자 (기본값 L'█')
   * @param fg_color 채워진 부분의 색상
   * @param bg_color 빈 부분의 색상
   * @param sorting_order 그리기 우선순위
   */
  void DrawProgressBar(int x, int y, int width, int height, float percentage,
                       wchar_t fill_char = L'█', Color fg_color = Color::kWhite,
                       Color bg_color = Color::kGray, int sorting_order = 0);

 private:
  Renderer& renderer_;  ///< 그리기를 위임받을 렌더러 참조
};

}  // namespace ui
}  // namespace mint

#endif  // MINT_UI_TEXT_LAYOUT_H_
