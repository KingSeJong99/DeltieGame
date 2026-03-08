#include "GuildLevel.h"

#include <algorithm>  // find를 위해 추가
#include <memory>     // smart_ptr
#include <string>

#include "Actor/Hero.h"
#include "Level/BattleLevel.h"
#include "MintEngine/Core/Color.h"
#include "MintEngine/Core/Input.h"
#include "MintEngine/Core/Logger.h"
#include "MintEngine/Engine/Engine.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Render/Renderer.h"
#include "MintUI/TextLayout.h"

namespace guild {

GuildLevel::GuildLevel() : selected_index_(0) {}

GuildLevel::~GuildLevel() {
  if (ui_layout_) {
    delete ui_layout_;
    ui_layout_ = nullptr;
  }
  MINT_LOG_INFO( L"레벨을 퇴장합니다.");
}

void GuildLevel::BeginPlay() {
  MINT_LOG_INFO(L"레벨을 불러오고 있어요.");
  Hero* hero1 = new Hero(L"시엘", 1000, 20, 10, 'S', L"S");
  Hero* hero2 = new Hero(L"용사", 800, 15, 5, 'A', L"H");
  Hero* hero3 = new Hero(L"델티", 500, 5, 1, 'C', L"D");

  AddNewActor(hero1);
  AddNewActor(hero2);
  AddNewActor(hero3);

  available_heroes_.push_back(hero1);
  available_heroes_.push_back(hero2);
  available_heroes_.push_back(hero3);
  MINT_LOG_INFO(L"레벨을 불러왔어요.");
}

void GuildLevel::Tick(float delta_time) {
  if (mint::Input::Get().GetKeyDown(VK_UP)) {
    // 음수 방지: 사이즈를 더하고 나머지 연산
    selected_index_ =
        (selected_index_ - 1 + static_cast<int>(available_heroes_.size())) %
        static_cast<int>(available_heroes_.size());
  }

  if (mint::Input::Get().GetKeyDown(VK_DOWN)) {
    // 음수 방지: 사이즈를 더하고 나머지 연산
    selected_index_ =
        (selected_index_ + 1) % static_cast<int>(available_heroes_.size());
  }

  if (mint::Input::Get().GetKeyDown(VK_SPACE)) {
    Hero* selected_hero = available_heroes_[selected_index_];

    // 용사 중복 체크 및 제거 (토글 방식)
    auto it = std::find(party_.begin(), party_.end(), selected_hero);
    if (it != party_.end()) {
      // 이미 파티에 있으면 제거
      MINT_LOG_INFO_TAG(selected_hero->name(), L"파티에서 제거 했습니다.");
      party_.erase(it);
    } else {
      // 파티에 없으면 추가 (사이즈 체크)
      if (party_.size() < kMaxPartySize) {
        MINT_LOG_INFO_TAG(selected_hero->name(), L"파티에 추가 했습니다.");
        party_.push_back(selected_hero);
      } else {
        MINT_LOG_WARN(L"파티가 이미 가득 찼습니다!");
      }
    }
  }

  // 레벨 전환
  if (mint::Input::Get().GetKeyDown(VK_RETURN)) {
    MINT_LOG_INFO(L"ENTER 입력 감지 - 레벨 전환 시도");
    MINT_LOG_INFO(L"현재 파티원 수: " + std::to_wstring(party_.size()));

    // 방어 코드
    if (party_.size() != kMaxPartySize) {
      MINT_LOG_WARN(L"파티원이 부족합니다! (필요: " +
                    std::to_wstring(kMaxPartySize) + L")");
      return;
    }

    MINT_LOG_INFO(L"파티를 구성하였습니다.");

    for (Hero* hero : party_) {
      RemoveActorWithoutDeleting(hero);
    }
    BattleLevel* battle_level = new BattleLevel(party_);
    mint::Engine::Get().SetNewLevel(battle_level);
  }
}
void GuildLevel::Draw(mint::Renderer& renderer, int width, int height) {
  if (!ui_layout_) {
    ui_layout_ = new mint::ui::TextLayout(renderer);
  }

  DrawUI(renderer);

  Level::Draw(renderer, width,
              height);  // 부모 클래스의 Draw 호출 (액터들 그리기)
}

void GuildLevel::DrawUI(mint::Renderer& renderer) {
  // UI 박스 그리기
  ui_layout_->DrawBox(10, 5, 25, 12, L"길드 본부 - 용사 선발",
                      mint::Color::kCyan);

  std::wstring status_text =
      L"선택 인덱스: " + std::to_wstring(selected_index_);
  ui_layout_->DrawTextAligned(12, 8, 21, status_text, mint::ui::Alignment::Left,
                              mint::Color::kWhite);

  // 3. 선택된 용사 이름 표시하기
  if (selected_index_ < static_cast<int>(available_heroes_.size())) {
    std::wstring hero_name =
        L"현재 선택: " + available_heroes_[selected_index_]->name();
    ui_layout_->DrawTextAligned(12, 10, 21, hero_name,
                                mint::ui::Alignment::Left,
                                mint::Color::kBrightCyan);
  }

  // 4. 선택된 용사 리스트 출력
  if (party_.empty()) {
    return;
  }

  ui_layout_->DrawTextAligned(12, 13, 21, L"현재 파티원:",
                              mint::ui::Alignment::Left, mint::Color::kGray);
  for (int idx = 0; idx < party_.size(); ++idx) {
    renderer.Submit(party_[idx]->name(), mint::Vector2(12 + (idx * 6), 14),
                    mint::Color::kBrightWhite);
  }
}
}  // namespace guild