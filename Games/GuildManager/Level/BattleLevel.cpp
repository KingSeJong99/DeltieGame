#include "BattleLevel.h"

#include <iostream>

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

  if (turn_manager_) {
    delete turn_manager_;
    turn_manager_ = nullptr;
  }
}

void BattleLevel::BeginPlay() {
  map_ = new mint::GridMap(40, 15);
  turn_manager_ = new mint::TurnManager();
  
  for (auto* hero : party_) {
    if (hero) hero->set_grid_map(map_);
  }

  // for (auto* enemy : enemies_) {
  //   if (enemy) enemy->set_map(map_);
  // }

  // 1. 아군 용사 등록 및 배치
  for (int i = 0; i < party_.size(); i++) {
    if (party_[i] == nullptr) continue;

    turn_manager_->AddParticipant(party_[i]);
    
    mint::Vector2 world_pos = map_->GridToWorld(5, 5 + (i * 2));
    world_pos.x += 6.0f;
    world_pos.y += 6.0f;

    party_[i]->set_position(world_pos);
    AddNewActor(party_[i]);
  }

  // 2. 적군 생성, 등록 및 배치
  Enemy* enemy1 = new Enemy(L"고블린", 50, 5, 2, L"G");
  Enemy* enemy2 = new Enemy(L"오크", 80, 10, 1, L"O");
  Enemy* enemy3 = new Enemy(L"트롤", 120, 15, 1, L"T");

  enemies_.push_back(enemy1);
  enemies_.push_back(enemy2);
  enemies_.push_back(enemy3);

  for (int i = 0; i < enemies_.size(); i++) {
    // 적군 리스트(enemies_)를 등록해야 함!!
    turn_manager_->AddParticipant(enemies_[i]);

    mint::Vector2 world_pos = map_->GridToWorld(35, 5 + (i * 2));
    world_pos.x += 6.0f;
    world_pos.y += 6.0f;

    enemies_[i]->set_position(world_pos);
    AddNewActor(enemies_[i]);
  }

  // 3. 첫 라운드 시작
  turn_manager_->StartNewRound();
  turn_manager_->NextTurn();
}

void BattleLevel::Tick(float delta_time) {
  // 아레나 경계 (UI 박스 안쪽)
  const float kMinX = 6.0f;
  const float kMaxX = 6.0f + static_cast<float>(map_->width()) - 1.0f;
  const float kMinY = 6.0f;
  const float kMaxY = 6.0f + static_cast<float>(map_->height()) - 1.0f;
 
  // 1. 타겟팅 매니저 (영웅과 적군을 1:1로 매칭시키거나 순차적으로 할당)
  for (int i = 0; i < party_.size(); ++i) {
    Hero* hero = party_[i];
    if (hero->is_dead() || hero->arena_info().state != CharacterState::kWaiting) continue; 
    
    // 이미 타겟이 있고 살아있으면 유지
    if (hero->target() && !dynamic_cast<Enemy*>(hero->target())->is_dead()) continue;

    // 새로운 타겟 할당 (단순하게 인덱스 기반 혹은 가까운 적)
    for (int j = 0; j < enemies_.size(); ++j) {
      int target_idx = (i + j) % enemies_.size();
      if (!enemies_[target_idx]->is_dead()) {
        hero->set_target(enemies_[target_idx]);
        break;
      }
    }
  }

  // for (Enemy* enemy : enemies_) {
  //   if (enemy->is_dead()) continue;
  //   // 적군도 타겟이 없으면 아군 중 하나를 선택
  //   if (!enemy->target() || dynamic_cast<Hero*>(enemy->target())->is_dead()) {
  //     for (Hero* hero : party_) {
  //       if (!hero->is_dead()) {
  //         enemy->set_target(hero);
  //         break;
  //       }
  //     }
  //   }
  // }

  // 2. 가두기 로직
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

  // 3. 턴 매니지먼트 로직
  mint::ITurnActor* current = turn_manager_->current_actor();

  if (current == nullptr || current->IsActionFinished()) {
    if (turn_manager_->NextTurn() == nullptr) {
      turn_manager_->StartNewRound();
      turn_manager_->NextTurn();
    }
  }
 
  // 4. 엔진 기본 틱 호출
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

  ui_layout_->DrawBox(5, 5, map_->width() + 2, map_->height() + 2, L"GRID ARENA", mint::Color::kRed);

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
