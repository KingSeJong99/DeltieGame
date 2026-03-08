#include "BattleLevel.h"

#include <iostream>

#include "Actor/Enemy.h"
#include "Actor/Hero.h"
#include "GuildLevel.h"
#include "MintEngine/Core/Logger.h"
#include "MintEngine/Engine/Engine.h"
#include "MintEngine/Level/GridMap.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Render/Renderer.h"
#include "MintUI/TextLayout.h"

namespace guild {

BattleLevel::BattleLevel(const std::vector<Hero*>& party)
    : party_(party),
      battle_timer_(0.0f),
      status_(BattleStatus::kInProgress),
      result_timer_(0.0f) {
  MINT_LOG_INFO(L"Level이 성공적으로 전환되었습니다.");
}

BattleLevel::~BattleLevel() {
  MINT_LOG_INFO(L"Level을 정리합니다.");
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
  MINT_LOG_INFO(L"맵과 용사들을 배치하고 있어요.");

  // 1. 맵 생성 (150x20으로 대폭 확장!)
  map_ = new mint::GridMap(150, 20);
  turn_manager_ = new mint::TurnManager();

  // 2. 장애물 배치 (중앙에 거대한 벽 생성)
  int mid_x = map_->width() / 2;  // 75
  for (int y = 4; y < 16; ++y) {
    map_->set_walkable(mid_x, y, false);
  }
  for (int x = mid_x - 10; x <= mid_x; ++x) {
    map_->set_walkable(x, 4, false);
    map_->set_walkable(x, 15, false);
  }

  // 3. 아군 용사 등록 및 배치 (왼쪽 끝)
  for (int i = 0; i < party_.size(); i++) {
    Hero* hero = party_[i];
    if (hero == nullptr) continue;

    hero->set_grid_map(map_);
    turn_manager_->AddParticipant(hero);

    mint::Vector2 world_pos = map_->GridToWorld(5, 7 + (i * 2));
    world_pos.x += 6.0f;
    world_pos.y += 6.0f;

    hero->set_position(world_pos);
    AddNewActor(hero);
  }

  // 4. 적군 생성 및 배치 (오른쪽 끝 145 위치)
  Enemy* enemy1 = new Enemy(L"고블린", 50, 5, 2, L"G");
  Enemy* enemy2 = new Enemy(L"오크", 80, 10, 1, L"O");
  Enemy* enemy3 = new Enemy(L"트롤", 120, 15, 1, L"T");

  enemies_.push_back(enemy1);
  enemies_.push_back(enemy2);
  enemies_.push_back(enemy3);

  for (int i = 0; i < enemies_.size(); i++) {
    enemies_[i]->set_grid_map(map_);
    turn_manager_->AddParticipant(enemies_[i]);

    mint::Vector2 world_pos = map_->GridToWorld(145, 7 + (i * 2));
    world_pos.x += 6.0f;
    world_pos.y += 6.0f;

    enemies_[i]->set_position(world_pos);
    AddNewActor(enemies_[i]);
  }

  turn_manager_->StartNewRound();
  MINT_LOG_INFO(L"세팅을 완료하였습니다.");
}

void BattleLevel::Tick(float delta_time) {
  if (status_ != BattleStatus::kInProgress) {
    result_timer_ += delta_time;
    if (result_timer_ >= 3.0f) {
      mint::Engine::Get().SetNewLevel(new GuildLevel());
      return;
    }
    Level::Tick(delta_time);
    return;
  }

  // 생존 확인
  bool any_hero_alive = false;
  for (auto* hero : party_)
    if (!hero->is_dead()) any_hero_alive = true;
  bool any_enemy_alive = false;
  for (auto* enemy : enemies_)
    if (!enemy->is_dead()) any_enemy_alive = true;

  if (!any_enemy_alive) {
    status_ = BattleStatus::kVictory;
    return;
  }
  if (!any_hero_alive) {
    status_ = BattleStatus::kDefeat;
    return;
  }

  // 아레나 경계 (확장된 맵 크기 반영)
  const float kMinX = 6.0f;
  const float kMaxX = 6.0f + static_cast<float>(map_->width()) - 1.0f;
  const float kMinY = 6.0f;
  const float kMaxY = 6.0f + static_cast<float>(map_->height()) - 1.0f;

  // 타겟팅 및 가두기
  for (int i = 0; i < party_.size(); ++i) {
    Hero* hero = party_[i];
    if (hero->is_dead() || hero->arena_info().state != CharacterState::kWaiting)
      continue;
    if (hero->target() && !dynamic_cast<Enemy*>(hero->target())->is_dead())
      continue;
    for (int j = 0; j < enemies_.size(); ++j) {
      int target_idx = (i + j) % enemies_.size();
      if (!enemies_[target_idx]->is_dead()) {
        hero->set_target(enemies_[target_idx]);
        break;
      }
    }
  }
  for (Enemy* enemy : enemies_) {
    if (enemy->is_dead()) continue;
    if (!enemy->target() || dynamic_cast<Hero*>(enemy->target())->is_dead()) {
      for (Hero* hero : party_) {
        if (!hero->is_dead()) {
          enemy->set_target(hero);
          break;
        }
      }
    }
  }

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

  // 턴 매니지먼트
  mint::ITurnActor* current = turn_manager_->current_actor();
  if (current == nullptr || current->IsActionFinished()) {
    current = turn_manager_->NextTurn();
    if (current == nullptr) {
      turn_manager_->StartNewRound();
      current = turn_manager_->NextTurn();
    }
  }

  Level::Tick(delta_time);
}

void BattleLevel::Draw(mint::Renderer& renderer, int width, int height) {
  if (!ui_layout_) ui_layout_ = new mint::ui::TextLayout(renderer);
  DrawBattleUI(renderer);
  Level::Draw(renderer, width, height);
}

void BattleLevel::DrawBattleUI(mint::Renderer& renderer) {
  if (!map_) return;

  // 1. 아레나 박스 그리기
  ui_layout_->DrawBox(5, 5, map_->width() + 2, map_->height() + 2,
                      L"EXPANDED ARENA", mint::Color::kRed, mint::Color::kBlack,
                      mint::Color::kWhite, -200);

  // 2. 전장 지형 렌더링 (바닥 및 장애물)
  for (int y = 0; y < map_->height(); ++y) {
    for (int x = 0; x < map_->width(); ++x) {
      mint::Vector2 screen_pos(6.0f + x, 6.0f + y);

      if (map_->tile(x, y).is_walkable) {
        // 이동 가능한 바닥은 밝게!
        renderer.Submit(L"█", screen_pos, mint::Color::kBrightYellow,
                        mint::Color::kBrightYellow, -100);
      } else {
        // 장애물은 회색으로!
        renderer.Submit(L"█", screen_pos, mint::Color::kDarkGray,
                        mint::Color::kBrightYellow, -50);
      }
    }
  }

  // 3. 파티 정보 (위치 조정)
  int info_y = 5 + map_->height() + 3;
  ui_layout_->DrawBox(5, info_y, 25, 6, L"내 파티 정보", mint::Color::kCyan);
  for (int i = 0; i < party_.size(); ++i) {
    if (party_[i] == nullptr) continue;
    renderer.Submit(party_[i]->name(), mint::Vector2(7, info_y + 1 + i),
                    mint::Color::kWhite);
    float hp_percent =
        static_cast<float>(party_[i]->hp()) / party_[i]->max_hp();
    ui_layout_->DrawProgressBar(
        14, info_y + 1 + i, 14, 1, hp_percent, L'■',
        (party_[i]->is_dead() ? mint::Color::kRed : mint::Color::kGreen),
        mint::Color::kGray);
  }

  ui_layout_->DrawBox(35, info_y, 25, 6, L"적군 정보", mint::Color::kMagenta);
  for (int i = 0; i < enemies_.size(); ++i) {
    if (enemies_[i] == nullptr) continue;
    renderer.Submit(enemies_[i]->name(), mint::Vector2(37, info_y + 1 + i),
                    mint::Color::kWhite);
    float hp_percent =
        static_cast<float>(enemies_[i]->hp()) / enemies_[i]->max_hp();
    ui_layout_->DrawProgressBar(
        44, info_y + 1 + i, 14, 1, hp_percent, L'■',
        (enemies_[i]->is_dead() ? mint::Color::kDarkGray : mint::Color::kRed),
        mint::Color::kGray);
  }

  // 결과 UI (생략 - 기존 로직 유지)
  if (status_ != BattleStatus::kInProgress) {
    int box_w = 30;
    int box_h = 7;
    int box_x = (map_->width() + 2 - box_w) / 2 + 5;
    int box_y = (map_->height() + 2 - box_h) / 2 + 5;
    mint::Color res_color =
        (status_ == BattleStatus::kVictory ? mint::Color::kYellow
                                           : mint::Color::kRed);
    ui_layout_->DrawBox(box_x, box_y, box_w, box_h, L" BATTLE RESULT ",
                        res_color, mint::Color::kBlack, res_color);
    ui_layout_->DrawTextAligned(
        box_x, box_y + 3, box_w,
        (status_ == BattleStatus::kVictory ? L"VICTORY!" : L"DEFEAT..."),
        mint::ui::Alignment::Center, res_color);
    ui_layout_->DrawTextAligned(
        box_x, box_y + 5, box_w, L"Returning to Guild...",
        mint::ui::Alignment::Center, mint::Color::kGray);
  }
}
}  // namespace guild
