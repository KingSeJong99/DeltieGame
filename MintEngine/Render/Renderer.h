#ifndef MINT_ENGINE_RENDER_RENDERER_H_
#define MINT_ENGINE_RENDER_RENDERER_H_

#include <Windows.h>

#include <string>
#include <vector>

#include "MintEngine/Core/Color.h"
#include "MintEngine/Math/Vector2.h"

namespace mint {

class ScreenBuffer;

/**
 * @brief 더블 버퍼링을 지원하는 콘솔 렌더러 클래스
 */
class MINT_API Renderer {
 public:
  /**
   * @brief 렌더러 생성자
   * @param screen_size 화면의 가로, 세로 크기
   */
  explicit Renderer(const Vector2& screen_size);

  /**
   * @brief 렌더러 소멸자
   */
  ~Renderer();

  /**
   * @brief 모든 렌더링 과정을 제어하는 함수 (Clear -> Render -> Present)
   */
  void Draw();

  /**
   * @brief 렌더 큐에 쌓인 명령들을 프레임 버퍼에 기록하는 함수
   */
  void Render();

  /**
   * @brief 그리기에 필요한 데이터를 제출하는 함수 (ASCII)
   * @param text 출력할 아스키 문자열
   * @param position 출력할 화면 좌표
   * @param color 텍스트 색상
   * @param sorting_order 그리기 우선순위
   * @note 내부적으로 UNICODE 버전의 Submit을 호출함
   */
  void Submit(const char* text, const Vector2& position,
              Color color = Color::kWhite, int sorting_order = 0);

  /**
   * @brief 그리기에 필요한 데이터를 제출하는 함수 (UNICODE)
   * @param text 출력할 유니코드 문자열
   * @param position 출력할 화면 좌표
   * @param color 텍스트 색상
   * @param sorting_order 그리기 우선순위
   */
  void Submit(std::wstring text, const Vector2& position,
              Color color = Color::kWhite, int sorting_order = 0);

  /**
   * @brief 문자열의 시각적 너비를 계산하는 정적 헬퍼 함수
   * @param text 너비를 계산할 문자열
   * @return 계산된 시각적 너비 (전각 문자는 2, 반각 문자는 1로 계산)
   */
  static int CalculateVisualWidth(const std::wstring& text);

  /**
   * @brief 현재 사용 중인 화면 버퍼를 반환
   * @return 현재 활성화된 ScreenBuffer 객체의 포인터
   */
  ScreenBuffer* GetCurrentBuffer() const;

  /**
   * @brief 프레임 버퍼의 CHAR_INFO 배열을 반환
   * @return 렌더링에 사용되는 CHAR_INFO 배열의 포인터
   */
  inline CHAR_INFO* GetFrameBuffer() const { return frame_->char_info_array_; }

  /**
   * @brief 화면 크기를 반환
   * @return 화면의 가로, 세로 크기 (Vector2)
   */
  inline const Vector2& GetScreenSize() const { return screen_size_; }

  /**
   * @brief 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
   */
  void Present();

  /**
   * @brief 화면과 프레임 버퍼를 초기화
   */
  void Clear();

  /**
   * @brief 지정된 위치에 단일 셀 정보를 설정
   * @param x X 좌표
   * @param y Y 좌표
   * @param ch 출력할 문자
   * @param foreground 전경색
   * @param background 배경색
   * @param sorting_order 그리기 우선순위
   */
  void SetCell(int x, int y, wchar_t ch, Color foreground, Color background,
               int sorting_order = 0);

 private:
  /**
   * @brief 프레임 데이터를 관리하는 내부 구조체
   */
  struct Frame {
    explicit Frame(int total_pixels);
    ~Frame();

    /**
     * @brief 프레임 버퍼 데이터를 초기화
     * @param screen_size 화면 크기
     */
    void Clear(const Vector2& screen_size);

    CHAR_INFO* char_info_array_ = nullptr;  ///< 색상 및 문자 정보를 담는 배열
    int* sorting_order_array_ = nullptr;    ///< 각 픽셀의 우선순위를 담는 배열
  };

  /**
   * @brief 렌더링 명령 구조체
   */
  struct RenderCommand {
    std::wstring text;      ///< 출력할 텍스트
    Vector2 position;       ///< 출력 위치
    Color color;            ///< 텍스트 색상
    int sorting_order = 0;  ///< 그리기 우선순위
  };

  Vector2 screen_size_;                      ///< 화면 크기
  Frame* frame_ = nullptr;                   ///< 현재 프레임 데이터 포인터
  ScreenBuffer* screen_buffers_[2];          ///< 이중 버퍼링용 스크린 버퍼 배열
  int current_buffer_index_ = 0;             ///< 현재 활성화된 버퍼 인덱스
  std::vector<RenderCommand> render_queue_;  ///< 렌더링 명령 큐

  /**
   * @brief 단일 렌더 커맨드를 프레임 버퍼에 그리는 헬퍼 함수
   */
  void DrawCommand(const RenderCommand& command);

  /**
   * @brief 단일 문자를 프레임 버퍼에 기록하는 헬퍼 함수
   */
  void DrawCharacter(int x, int y, wchar_t ch, Color color, int sorting_order);

  /**
   * @brief 문자가 전각 문자인지 판별하는 헬퍼 함수
   */
  static bool IsWideChar(wchar_t ch);
};

}  // namespace mint

#endif  // MINT_ENGINE_RENDER_RENDERER_H_
