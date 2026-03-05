#include "BattleLevel.h"

#include "Actor/Enemy.h"
#include "Actor/Hero.h"
#include "MintEngine/Level/GridMap.h"
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

  if (map_) {
    delete map_;
    map_ = nullptr;
  }
}

void BattleLevel::BeginPlay() {
  // 1. 격자 맵 생성 (가로 40, 세로 15 칸)
  map_ = new mint::GridMap(40, 15);

  // 2. 우리 파티 용사들을 격자 좌표 기준으로 배치
  for (int i = 0; i < party_.size(); i++) {
    if (party_[i] == nullptr) continue;

    // 격자 좌표 (5, 5+i*2)를 월드 좌표로 변환하여 설정
    mint::Vector2 world_pos = map_->GridToWorld(5, 5 + (i * 2));
    
    // UI 박스 위치(x=5, y=5)를 고려한 오프셋
    world_pos.x += 6.0f;
    world_pos.y += 6.0f;
    
    party_[i]->set_position(world_pos);
    AddNewActor(party_[i]);
  }

  // 3. 적군 생성 및 배치
  Enemy* enemy1 = new Enemy(L"고블린", 50, 5, 2, L"G");
  Enemy* enemy2 = new Enemy(L"오크", 80, 10, 1, L"O");
  Enemy* enemy3 = new Enemy(L"트롤", 120, 15, 1, L"T");

  enemies_.push_back(enemy1);
  enemies_.push_back(enemy2);
  enemies_.push_back(enemy3);

  for (int i = 0; i < enemies_.size(); i++) {
    // 격자 좌표 (35, 5+i*2)를 월드 좌표로 변환
    mint::Vector2 world_pos = map_->GridToWorld(35, 5 + (i * 2));
    
    world_pos.x += 6.0f;
    world_pos.y += 6.0f;

    enemies_[i]->set_position(world_pos);
    AddNewActor(enemies_[i]);
  }
}

void BattleLevel::Tick(float delta_time) {
  // 아레나 논리적 경계 (UI 박스 안쪽)
  const float kMinX = 6.0f;
  const float kMaxX = 6.0f + static_cast<float>(map_->width()) - 1.0f;
  const float kMinY = 6.0f;
  const float kMaxY = 6.0f + static_cast<float>(map_->height()) - 1.0f;

  // 1. 타겟팅 매니저
  for (Hero* hero : party_) {
    if (hero->is_dead()) continue;
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

  // 2. 모든 액터를 아레나 안에 가두기
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

  // 3. 부모 클래스의 Tick 호출
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
  if (!map_) return;

  // 1. 메인 전장 (GridMap 크기에 맞춰 박스 그리기)
  // 박스 위치 (5, 5), 가로폭은 맵너비+2, 세로폭은 맵높이+2
  ui_layout_->DrawBox(5, 5, map_->width() + 2, map_->height() + 2, L"GRID ARENA", mint::Color::kRed);

  // 2. 하단 현황판
  int info_y = 5 + map_->height() + 3;
  ui_layout_->DrawBox(5, info_y, 25, 6, L"내 파티 정보", mint::Color::kCyan);
  for (int i = 0; i < party_.size(); ++i) {
    if (party_[i] == nullptr) continue;
    renderer.Submit(party_[i]->name(), mint::Vector2(7, info_y + 1 + i), mint::Color::kWhite);
    float hp_percent = static_cast<float>(party_[i]->hp()) / party_[i]->max_hp();
    mint::Color hp_color = (party_[i]->is_dead()) ? mint::Color::kRed : mint::Color::kGreen;
    ui_layout_->DrawProgressBar(14, info_y + 1 + i, 14, 1, hp_percent, L'■', hp_color, mint::Color::kGray);
  }

  ui_layout_->DrawBox(35, info_y, 25, 6, L"적군 정보", mint::Color::kMagenta);
  for (int i = 0; i < enemies_.size(); ++i) {
    if (enemies_[i] == nullptr) continue;
    renderer.Submit(enemies_[i]->name(), mint::Vector2(37, info_y + 1 + i), mint::Color::kWhite);
    float hp_percent = static_cast<float>(enemies_[i]->hp()) / enemies_[i]->max_hp();
    mint::Color hp_color = (enemies_[i]->is_dead()) ? mint::Color::kDarkGray : mint::Color::kRed;
    ui_layout_->DrawProgressBar(44, info_y + 1 + i, 14, 1, hp_percent, L'■', hp_color, mint::Color::kGray);
  }
}
}  // namespace guild
