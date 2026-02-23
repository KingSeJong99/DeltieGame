#include "MintEngine/Render/Renderer.h"

#include "MintEngine/Core/Util.h"
#include "MintEngine/Render/ScreenBuffer.h"

namespace mint {

Renderer::Frame::Frame(int total_pixels)
    : char_info_array_(new CHAR_INFO[total_pixels]()),
      sorting_order_array_(new int[total_pixels]()) {}

Renderer::Frame::~Frame() {
  SafeDeleteArray(char_info_array_);
  SafeDeleteArray(sorting_order_array_);
}

void Renderer::Frame::Clear(const Vector2& screen_size) {
  const int width = static_cast<int>(screen_size.x);
  const int height = static_cast<int>(screen_size.y);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const int index = (y * width) + x;

      CHAR_INFO& info = char_info_array_[index];
      info.Char.UnicodeChar = L' ';
      info.Attributes = 0;

      sorting_order_array_[index] = -1;
    }
  }
}

Renderer::Renderer(const Vector2& screen_size) : screen_size_(screen_size) {
  const int buffer_count = static_cast<int>(screen_size_.x * screen_size_.y);
  frame_ = new Frame(buffer_count);
  frame_->Clear(screen_size_);

  screen_buffers_[0] = new ScreenBuffer(screen_size_);
  screen_buffers_[0]->Clear();

  screen_buffers_[1] = new ScreenBuffer(screen_size_);
  screen_buffers_[1]->Clear();

  Present();
}

Renderer::~Renderer() {
  SafeDelete(frame_);
  for (ScreenBuffer*& buffer : screen_buffers_) {
    SafeDelete(buffer);
  }
}

void Renderer::DrawCharacter(int x, int y, wchar_t ch, Color color,
                             int sorting_order) {
  SetCell(x, y, ch, color, Color::kBlack, sorting_order);
}

bool Renderer::IsWideChar(wchar_t ch) {
  if ((ch >= 0x1100 && ch <= 0x11FF) ||  // Hangul Jamo
      (ch >= 0xAC00 && ch <= 0xD7AF) ||  // Hangul Syllables
      (ch >= 0x3000 && ch <= 0x303F) ||  // CJK Symbols
      //(ch >= 0x2000 && ch <= 0x2BFF) ||  // 박스 기호 및 각종 특수 기호 범위 추가
      (ch >= 0x4E00 && ch <= 0x9FFF) ||  // CJK Unified Ideographs
      (ch >= 0xFF00 && ch <= 0xFFEF))    // Fullwidth Forms
  {
    return true;
  }
  return false;
}

void Renderer::SetCell(int x, int y, wchar_t ch, Color foreground,
                       Color background, int sorting_order) {
  if (x < 0 || x >= static_cast<int>(screen_size_.x) || 
      y < 0 || y >= static_cast<int>(screen_size_.y)) {
    return;
  }

  const int index = (y * static_cast<int>(screen_size_.x)) + x;

  if (frame_->sorting_order_array_[index] > sorting_order) {
    return;
  }

  int char_width = IsWideChar(ch) ? 2 : 1;
  WORD attributes = static_cast<WORD>(foreground) | 
                    (static_cast<WORD>(background) << 4);

  if (char_width == 2) {
    if (x + 1 < static_cast<int>(screen_size_.x)) {
      frame_->char_info_array_[index].Char.UnicodeChar = ch;
      frame_->char_info_array_[index].Attributes =
          attributes | COMMON_LVB_LEADING_BYTE;
      frame_->sorting_order_array_[index] = sorting_order;

      frame_->char_info_array_[index + 1].Char.UnicodeChar = ch;
      frame_->char_info_array_[index + 1].Attributes =
          attributes | COMMON_LVB_TRAILING_BYTE;
      frame_->sorting_order_array_[index + 1] = sorting_order;
    }
  } else {
    frame_->char_info_array_[index].Char.UnicodeChar = ch;
    frame_->char_info_array_[index].Attributes = attributes;
    frame_->sorting_order_array_[index] = sorting_order;
  }
}

void Renderer::DrawCommand(const RenderCommand& command) {
  if (command.text.empty() || command.position.y < 0 ||
      command.position.y >= static_cast<int>(screen_size_.y)) {
    return;
  }

  int target_x = static_cast<int>(command.position.x);
  const int target_y = static_cast<int>(command.position.y);

  for (const wchar_t ch : command.text) {
    int char_width = IsWideChar(ch) ? 2 : 1;

    if (target_x >= static_cast<int>(screen_size_.x)) {
      break;
    }

    if (target_x < 0) {
      target_x += char_width;
      continue;
    }

    DrawCharacter(target_x, target_y, ch, command.color, command.sorting_order);
    target_x += char_width;
  }
}

void Renderer::Draw() {
  Clear();
  for (const RenderCommand& command : render_queue_) {
    DrawCommand(command);
  }
  render_queue_.clear();
  Present();
}

void Renderer::Render() {
  for (const RenderCommand& command : render_queue_) {
    DrawCommand(command);
  }
  render_queue_.clear();
}

void Renderer::Clear() {
  frame_->Clear(screen_size_);
  GetCurrentBuffer()->Clear();
}

void Renderer::Submit(const char* text, const Vector2& position, Color color,
                      int sorting_order) {
  if (text == nullptr) return;

  std::string s(text);
  std::wstring ws(s.begin(), s.end());
  
  Submit(std::move(ws), position, color, sorting_order);
}

void Renderer::Submit(std::wstring text, const Vector2& position, Color color,
                      int sorting_order) {
  if (text.empty()) return;

  RenderCommand command = {};
  command.text = std::move(text);
  command.position = position;
  command.color = color;
  command.sorting_order = sorting_order;

  render_queue_.emplace_back(std::move(command));
}

int Renderer::CalculateVisualWidth(const std::wstring& text) {
  int width = 0;
  for (wchar_t ch : text) {
    width += IsWideChar(ch) ? 2 : 1;
  }
  return width;
}

void Renderer::Present() {
  auto* target_buffer = GetCurrentBuffer();
  if (!target_buffer) return;

  HANDLE h_out = target_buffer->GetBuffer();

  COORD buffer_size = {static_cast<short>(screen_size_.x),
                       static_cast<short>(screen_size_.y)};
  COORD buffer_coord = {0, 0};
  SMALL_RECT write_region = {0, 0, 
                             static_cast<short>(screen_size_.x - 1),
                             static_cast<short>(screen_size_.y - 1)};

  BOOL success = WriteConsoleOutputW(h_out, GetFrameBuffer(), buffer_size,
                                     buffer_coord, &write_region);

  if (!success) {
    // DWORD error = GetLastError();
    __debugbreak();
  }

  SetConsoleActiveScreenBuffer(h_out);
  current_buffer_index_ = 1 - current_buffer_index_;
}

ScreenBuffer* Renderer::GetCurrentBuffer() const {
  return screen_buffers_[current_buffer_index_];
}

}  // namespace mint
