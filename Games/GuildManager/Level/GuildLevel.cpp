#include "GuildLevel.h"

#include <memory>  // smart_ptr
#include <string>

#include "Actor/Hero.h"
#include "Level/BattleLevel.h"
#include "MintEngine/Core/Color.h"
#include "MintEngine/Core/Input.h"
#include "MintEngine/Engine/Engine.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Render/Renderer.h"

namespace guild {

GuildLevel::GuildLevel() : selected_index_(0) {}

GuildLevel::~GuildLevel() { 
    // 부모 클래스의 소멸자가 actors_에 있는 객체를 delete 하기 전에
    // 리스트를 비워서 hero를 소멸하지 않도록 한다
    actors_.clear(); }

void GuildLevel::BeginPlay() {
  Hero* hero1 = new Hero(L"시엘", 100, 20, 10, 'S', L"C");
  Hero* hero2 = new Hero(L"용사", 80, 15, 5, 'A', L"H");
  Hero* hero3 = new Hero(L"델티", 50, 5, 1, 'C', L"D");

  AddNewActor(hero1);
  AddNewActor(hero2);
  AddNewActor(hero3);

  available_heroes_.push_back(hero1);
  available_heroes_.push_back(hero2);
  available_heroes_.push_back(hero3);
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

    // 용사 중복 체크
    bool already_in_party = false;
    for (Hero* hero : party_) {
      if (hero == selected_hero) {
        already_in_party = true;
        break;
      }
    }

    // 파티가 찼는지 확인
    if (party_.size() >= kMaxPartySize) {
      // Todo: 로그나 UI를 제공해 더이상 선택이 불가능함을 알려야함
      return;
    }

    // 조건을 통과하면 파티에 추가하기
    if (!already_in_party) {
      party_.push_back(selected_hero);
      mint::Color color = mint::Color::kBrightYellow;
    }
  }

  // 레벨 전환
  if (mint::Input::Get().GetKeyDown(VK_RETURN)) {
    // 방어 코드
    if (party_.size() != kMaxPartySize) return;

    BattleLevel* battle_level = new BattleLevel(party_);

    mint::Engine::Get().SetNewLevel(battle_level);
  }
}
void GuildLevel::Draw(mint::Renderer& renderer, int width, int height) {
  DrawUI(renderer);

  Level::Draw(renderer, width,
              height);  // 부모 클래스의 Draw 호출 (액터들 그리기)
}

void GuildLevel::DrawUI(mint::Renderer& renderer) {
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

  // 2. 한글 출력 테스트 (전각 문자 테스트)
  renderer.Submit(std::wstring(L"길드 본부 - 용사 선발"), mint::Vector2(12, 7),
                  mint::Color::kYellow);

  std::wstring status_text =
      L"선택된 용사 인덱스: " + std::to_wstring(selected_index_);
  renderer.Submit(std::move(status_text), mint::Vector2(12, 9),
                  mint::Color::kWhite);

  // 3. 선택된 용사 이름 표시하기
  if (selected_index_ < static_cast<int>(available_heroes_.size())) {
    std::wstring hero_name =
        L"현재 선택: " + available_heroes_[selected_index_]->name();
    renderer.Submit(hero_name, mint::Vector2(12, 11), mint::Color::kBrightCyan);
  }

  // 4. 선택된 용사 리스트 출력
  if (static_cast<int>(party_.empty())) {
    return;
  }

  renderer.Submit(L"현재 파티원:", mint::Vector2(12, 13), mint::Color::kGray);
  for (int idx = 0; idx < party_.size(); ++idx) {
    renderer.Submit(party_[idx]->name(), mint::Vector2(12 + (idx * 6), 14),
                    mint::Color::kBrightWhite);
  }
}
}  // namespace guild
