#include "MintEngine/Actor/Actor.h"

#include <Windows.h>

#include "MintEngine/Core/Logger.h"
#include "MintEngine/Core/Util.h"
#include "MintEngine/Render/Renderer.h"

namespace mint {

Actor::Actor(const std::wstring& name, const std::wstring& image,
             const Vector2& position, Color color)
    : name_(name), image_(image), color_(color), position_(position) {
  MINT_LOG_DEBUG_TAG(name_, L"액터가 생성되었어요.");
}

Actor::~Actor() { MINT_LOG_DEBUG_TAG(name_, L"액터가 소멸되었어요."); }

void Actor::BeginPlay() {
  has_began_play_ = true;
  MINT_LOG_INFO_TAG(name_, L"BeginPlay 수행합니다.");
}

void Actor::Tick(float delta_time) {}

void Actor::Draw(Renderer& renderer, int width, int height) {
  if (!image_.empty()) {
    renderer.Submit(image_, position_, color_, Color::kBlack, sorting_order_);
  }
}

void Actor::set_position(const Vector2& new_position) {
  if (position_ == new_position) {
    return;
  }
  position_ = new_position;
}

}  // namespace mint
