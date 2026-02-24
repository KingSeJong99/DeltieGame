#include "MintEngine/Render/ScreenBuffer.h"

#include <iostream>

#include "MintEngine/Render/Renderer.h"

namespace mint {

ScreenBuffer::ScreenBuffer(const Vector2& screen_size)
    : screen_size_(screen_size) {
  // Console Output 생성하기
  buffer_ = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);

  // 예외 처리하기
  if (buffer_ == INVALID_HANDLE_VALUE) {
    MessageBoxA(nullptr, "ScreenBuffer - Failed to create buffer.",
                "Buffer creation error", MB_OK);
    __debugbreak();
  }

  // 콘솔 창 크기 지정하기
  SMALL_RECT rect;
  rect.Left = 0;
  rect.Top = 0;
  rect.Right = static_cast<short>(screen_size_.x - 1);
  rect.Bottom = static_cast<short>(screen_size_.y - 1);

  if (!SetConsoleWindowInfo(buffer_, TRUE, &rect)) {
    std::cerr << "Failed to set console window info.\n";
    __debugbreak();
  }

  // 버퍼 크기 설정하기
  COORD coord = {static_cast<short>(screen_size_.x),
                 static_cast<short>(screen_size_.y)};
  if (!SetConsoleScreenBufferSize(buffer_, coord)) {
    std::cerr << "Failed to set console buffer size.\n";
    __debugbreak();
  }

  // 커서 끄기
  CONSOLE_CURSOR_INFO info;
  GetConsoleCursorInfo(buffer_, &info);

  // 끄도록 설정하기
  info.bVisible = FALSE;
  SetConsoleCursorInfo(buffer_, &info);
}

ScreenBuffer::~ScreenBuffer() {
  // 버퍼 해제
  if (buffer_) {
    CloseHandle(buffer_);
  }
}

void ScreenBuffer::Clear() {
  // 실제로 화면을 지우고 난 뒤에 몇 글자를 썼는지 반환한다
  DWORD written_count = 0;

  // 콘솔 버퍼에 있는 화면 지우기 (한 색상으로 덮어쓰기)
  COORD zero_coord = {0, 0};
  FillConsoleOutputCharacterA(
      buffer_, ' ',
      static_cast<DWORD>(screen_size_.x * screen_size_.y),
      zero_coord, &written_count);
}

void ScreenBuffer::Draw(const CHAR_INFO* char_info) {
  // 설정할 버퍼의 크기
  SMALL_RECT write_region = {};
  write_region.Left = 0;
  write_region.Top = 0;
  write_region.Right = static_cast<short>(screen_size_.x - 1);
  write_region.Bottom = static_cast<short>(screen_size_.y - 1);

  COORD buffer_size = {static_cast<short>(screen_size_.x),
                       static_cast<short>(screen_size_.y)};
  COORD buffer_coord = {0, 0};

  // 버퍼에 받은 글자 배열 설정
  WriteConsoleOutputW(buffer_, char_info, buffer_size, buffer_coord,
                      &write_region);
}

}  // namespace mint
