#include "BattleLevel.h"

#include "Actor/Enemy.h"
#include "Actor/Hero.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Render/Renderer.h"
#include "MintUI/TextLayout.h"

namespace guild {

BattleLevel::BattleLevel(const std::vector<Hero*>& party)
    : party_(party), battle_timer_(0.0f) {}

BattleLevel::~BattleLevel() {
  if (ui_layout_) {
    delete ui_layout_;
    ui_layout_ = nullptr;
  }
}

void BattleLevel::BeginPlay() {
  // 1. 우리 파티 용사들을 전장에 배치 및 등록하기
  for (int i = 0; i < party_.size(); i++) {
    if (party_[i] == nullptr) continue;

    mint::Vector2 position(15.0f, 10.0f + (i * 2.0f));
    party_[i]->set_position(position);

    AddNewActor(party_[i]);
  }

  // 2. 적군 생성 및 전장 배치
  Enemy* enemy1 = new Enemy(L"고블린", 50, 5, 2, L"G");
  Enemy* enemy2 = new Enemy(L"오크", 80, 10, 1, L"O");
  Enemy* enemy3 = new Enemy(L"트롤", 120, 15, 1, L"T");

  enemies_.push_back(enemy1);
  enemies_.push_back(enemy2);
  enemies_.push_back(enemy3);

  for (int i = 0; i < enemies_.size(); i++) {
    mint::Vector2 position(60.0f, 10.0f + (i * 2.0f));
    enemies_[i]->set_position(position);
    AddNewActor(enemies_[i]);
  }
}

void BattleLevel::Tick(float delta_time) {
  // 아레나 논리적 경계
  const float kMinX = 6.0f;
  const float kMaxX = 74.0f;
  const float kMinY = 6.0f;
  const float kMaxY = 19.0f;

  // 1. 타겟팅 매니저: 살아있는 용사와 적들에게 상대를 매칭해준다.
  for (Hero* hero : party_) {
    if (hero->is_dead()) continue;

    // 이미 타겟이 있으면 패스

    // 가장 가까운 혹은 살아있는 첫 번째 적을 타겟으로 지정
    for (Enemy* enemy : enemies_) {
      if (!enemy->is_dead()) {
        hero->set_target(enemy);
        break;
      }
    }
  }

  for (Enemy* enemy : enemies_) {
    if (enemy->is_dead()) continue;

    for (Hero* hero : party_) {
      if (!hero->is_dead()) {
        enemy->set_target(hero);
        break;
      }
    }
  }

  // 2. 모든 액터를 아레나 안에 가두기 (Clamping)
  auto clamp_actor = [&](mint::Actor* actor) {
    if (!actor) return;
    mint::Vector2 pos = actor->position();
    if (pos.x < kMinX) pos.x = kMinX;
    if (pos.x > kMaxX) pos.x = kMaxX;
    if (pos.y < kMinY) pos.y = kMinY;
    if (pos.y > kMaxY) pos.y = kMaxY;
    actor->set_position(pos);
  };

  for (auto* hero : party_) clamp_actor(hero);
  for (auto* enemy : enemies_) clamp_actor(enemy);

  // 3. 액터들의 Tick실행!
  Level::Tick(delta_time);
}

void BattleLevel::Draw(mint::Renderer& renderer, int width, int height) {
  if (!ui_layout_) {
    ui_layout_ = new mint::ui::TextLayout(renderer);
  }

  DrawBattleUI(renderer);

  Level::Draw(renderer, width, height);
}

void BattleLevel::DrawBattleUI(mint::Renderer& renderer) {
  ui_layout_->DrawBox(5, 5, 71, 16, L"BATTLE ARENA", mint::Color::kRed);

  ui_layout_->DrawBox(5, 22, 25, 6, L"내 파티 정보", mint::Color::kCyan);
  for (int i = 0; i < party_.size(); ++i) {
    if (party_[i] == nullptr) continue;
    renderer.Submit(party_[i]->name(), mint::Vector2(7, 23 + i), mint::Color::kWhite);
    float hp_percent = static_cast<float>(party_[i]->hp()) / party_[i]->max_hp();
    mint::Color hp_color = (party_[i]->is_dead()) ? mint::Color::kRed : mint::Color::kGreen;
    ui_layout_->DrawProgressBar(14, 23 + i, 14, 1, hp_percent, L'■', hp_color, mint::Color::kGray);
  }

  ui_layout_->DrawBox(50, 22, 25, 6, L"적군 정보", mint::Color::kMagenta);
  for (int i = 0; i < enemies_.size(); ++i) {
    if (enemies_[i] == nullptr) continue;
    renderer.Submit(enemies_[i]->name(), mint::Vector2(52, 23 + i), mint::Color::kWhite);
    float hp_percent = static_cast<float>(enemies_[i]->hp()) / enemies_[i]->max_hp();
    mint::Color hp_color = (enemies_[i]->is_dead()) ? mint::Color::kDarkGray : mint::Color::kRed;
    ui_layout_->DrawProgressBar(59, 23 + i, 14, 1, hp_percent, L'■', hp_color, mint::Color::kGray);
  }
}
}  // namespace guild
