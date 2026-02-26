#include "BattleLevel.h"

#include "Actor/Hero.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Render/Renderer.h"

namespace guild {

BattleLevel::BattleLevel(const std::vector<Hero*>& party) : party_(party) {}

BattleLevel::~BattleLevel() = default;

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
  DrawBattleUI(renderer);
  // Todo: 용사를 그리는 함수
  // 

  Level::Draw(renderer, width, height);
}

void BattleLevel::DrawBattleUI(mint::Renderer& renderer) {
  // Todo: 박스 그리기가 반복될 확률이 높으므로 MintEngine 프로젝트에서 UI를 다루는 녀석을 만들거나
  // GuildManager 프로젝트에서 만들거나 UI 전용 프로젝트를 만들어 관리하는 것도 보임. 우선은 구현이 목적이므로 구현에 우선함
  // 
  // UI 박스 그리기
  // 상단
  renderer.SetCell(10, 5, L'┏', mint::Color::kCyan, mint::Color::kBlack);
  for (int i = 11; i < 30; ++i)
    renderer.SetCell(i, 5, L'━', mint::Color::kCyan, mint::Color::kBlack);
  renderer.SetCell(30, 5, L'┓', mint::Color::kCyan, mint::Color::kBlack);

  // 측면
  for (int i = 6; i < 15; ++i) {
    renderer.SetCell(10, i, L'┃', mint::Color::kCyan, mint::Color::kBlack);
    renderer.SetCell(30, i, L'┃', mint::Color::kCyan, mint::Color::kBlack);
  }

  // 하단
  renderer.SetCell(10, 15, L'┗', mint::Color::kCyan, mint::Color::kBlack);
  for (int i = 11; i < 30; ++i)
    renderer.SetCell(i, 15, L'━', mint::Color::kCyan, mint::Color::kBlack);
  renderer.SetCell(30, 15, L'┛', mint::Color::kCyan, mint::Color::kBlack);

  renderer.Submit(L"전투 시작~ 모두들 힘내.", mint::Vector2(12, 10),
                  mint::Color::kBrightRed);
}
}  // namespace guild