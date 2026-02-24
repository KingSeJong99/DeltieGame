#include "MintEngine/Actor/Actor.h"

#include <Windows.h>

#include "MintEngine/Core/Util.h"
#include "MintEngine/Render/Renderer.h"

namespace mint {

Actor::Actor(const std::wstring& image, const Vector2& position, Color color)
    : image_(image), color_(color), position_(position) {}

Actor::~Actor() {}

void Actor::BeginPlay() {
  has_began_play_ = true;
}

void Actor::Tick(float delta_time) {}

void Actor::Draw(CHAR_INFO* back_buffer, int width, int height) {
  int x = static_cast<int>(position_.x);
  int y = static_cast<int>(position_.y);

  if (x < 0 || x >= width || y < 0 || y >= height) {
    return;
  }

  int index = y * width + x;

  if (!image_.empty()) {
    // 유니코드 문자열의 첫 번째 문자를 출력
    back_buffer[index].Char.UnicodeChar = image_[0];
  }
  back_buffer[index].Attributes = static_cast<WORD>(color_);
}

void Actor::set_position(const Vector2& new_position) {
  if (position_ == new_position) {
    return;
  }
  position_ = new_position;
}

}  // namespace mint
