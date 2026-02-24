#include "GuildLevel.h"

#include <memory>  // smart_ptr

#include "Actor/Hero.h"
#include "MintEngine/Core/Color.h"
#include "MintEngine/Core/Input.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Render/Renderer.h"

namespace guild {

GuildLevel::GuildLevel() : selected_index_(0) {}

GuildLevel::~GuildLevel() {}

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
  // Todo: 키 입력을 받아 selected_index_를 변경하는 로직 추가 예정
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
}
}  // namespace guild
