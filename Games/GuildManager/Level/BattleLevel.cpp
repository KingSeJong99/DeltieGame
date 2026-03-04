#include "BattleLevel.h"

#include "Actor/Hero.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Render/Renderer.h"
#include "UI/TextLayout.h"

namespace guild {

BattleLevel::BattleLevel(const std::vector<Hero*>& party) : party_(party) {}

BattleLevel::~BattleLevel() {
  if (ui_layout_) {
    delete ui_layout_;
    ui_layout_ = nullptr;
  }
}

void BattleLevel::BeginPlay() {
  
  // 우리 파티 용사들을 전장에 배치 및 등록하기
  for (int i = 0; i < party_.size(); i++) {
    if (party_[i] == nullptr) continue;

    mint::Vector2 position(15.0f, 18.0f + (i * 2.0f));
    party_[i]->set_position(position);

    AddNewActor(party_[i]);
  }
}

void BattleLevel::Tick(float delta_time) {
    
    // for문 돌리며 용사들 움직이게 하기
}

void BattleLevel::Draw(mint::Renderer& renderer, int width, int height) {
  if (!ui_layout_) {
    ui_layout_ = new mint::ui::TextLayout(renderer);
  }

  DrawBattleUI(renderer);
  // Todo: 용사를 그리는 함수
  // 

  Level::Draw(renderer, width, height);
}

void BattleLevel::DrawBattleUI(mint::Renderer& renderer) {
  // 상단 현황판
  ui_layout_->DrawBox(5, 2, 70, 5, L"전투 필드", mint::Color::kRed);
  ui_layout_->DrawTextAligned(7, 4, 66, L"적을 소탕하십시오!", mint::ui::Alignment::Center, mint::Color::kYellow);

  // 우리 파티 상태 정보
  ui_layout_->DrawBox(5, 18, 25, 6, L"내 파티 정보", mint::Color::kCyan);
  
  for (int i = 0; i < party_.size(); ++i) {
    if (party_[i] == nullptr) continue;

    // 이름 표시
    renderer.Submit(party_[i]->name(), mint::Vector2(7, 19 + i), mint::Color::kWhite);
    
    // HP바 표시
    float hp_percent = static_cast<float>(party_[i]->hp()) / party_[i]->max_hp();
    ui_layout_->DrawProgressBar(14, 19 + i, 14, 1, hp_percent, L'■', mint::Color::kGreen, mint::Color::kGray);
  }
}
}  // namespace guild

}  // namespace guild