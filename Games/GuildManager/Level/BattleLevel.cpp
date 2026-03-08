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
  MINT_LOG_INFO(L"Level을 정리합니다. (주소: " +
                std::to_wstring((uintptr_t)this) + L")");
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
  map_ = new mint::GridMap(40, 15);
  turn_manager_ = new mint::TurnManager();

  // 1. 아군 용사 등록 및 배치
  for (int i = 0; i < party_.size(); i++) {
    Hero* hero = party_[i];
    if (hero == nullptr) {
      MINT_LOG_WARN(L"용사가 비어있습니다!");
      continue;
    }
    hero->set_grid_map(map_);
    turn_manager_->AddParticipant(hero);

    mint::Vector2 world_pos = map_->GridToWorld(5, 5 + (i * 2));
    world_pos.x += 6.0f;
    world_pos.y += 6.0f;

    hero->set_position(world_pos);
    AddNewActor(hero);
    MINT_LOG_DEBUG_TAG(hero->name(),
                       L"용사를 배치했어요. 위치: (" +
                           std::to_wstring((int)world_pos.x) + L", " +
                           std::to_wstring((int)world_pos.y) + L")");
  }

  // 2. 적군 생성, 등록 및 배치
  Enemy* enemy1 = new Enemy(L"고블린", 50, 5, 2, L"G");
  Enemy* enemy2 = new Enemy(L"오크", 80, 10, 1, L"O");
  Enemy* enemy3 = new Enemy(L"트롤", 120, 15, 1, L"T");

  enemies_.push_back(enemy1);
  enemies_.push_back(enemy2);
  enemies_.push_back(enemy3);

  for (int i = 0; i < enemies_.size(); i++) {
    enemies_[i]->set_grid_map(map_);
    turn_manager_->AddParticipant(enemies_[i]);

    mint::Vector2 world_pos = map_->GridToWorld(35, 5 + (i * 2));
    world_pos.x += 6.0f;
    world_pos.y += 6.0f;

    enemies_[i]->set_position(world_pos);
    AddNewActor(enemies_[i]);
  }

  // 3. 첫 라운드 시작
  turn_manager_->StartNewRound();
  MINT_LOG_INFO(L"세팅을 완료하였습니다.");
}

void BattleLevel::Tick(float delta_time) {
  // 0. 전투 결과 판정 및 타이머 처리
  if (status_ != BattleStatus::kInProgress) {
    result_timer_ += delta_time;
    if (result_timer_ >= 3.0f) {
      MINT_LOG_INFO(L"전투가 종료되었어요.");
      mint::Engine::Get().SetNewLevel(new GuildLevel());
      return;
    }
    
    Level::Tick(delta_time);
    return;
  }

  // 1. 생존 확인 (전멸 판정)
  bool any_hero_alive = false;
  for (auto* hero : party_) {
    if (!hero->is_dead()) {
      any_hero_alive = true;
      break;
    }
  }

  bool any_enemy_alive = false;
  for (auto* enemy : enemies_) {
    if (!enemy->is_dead()) {
      any_enemy_alive = true;
      break;
    }
  }

  if (!any_enemy_alive) {
    MINT_LOG_INFO(L"플레이어가 승리했습니다!");
    status_ = BattleStatus::kVictory;
    return;
  }

  if (!any_hero_alive) {
    MINT_LOG_INFO(L"플레이어가 패배했습니다!");
    status_ = BattleStatus::kDefeat;
    return;
  }

  // 아레나 경계 (UI 박스 안쪽)
  const float kMinX = 6.0f;
  const float kMaxX = 6.0f + static_cast<float>(map_->width()) - 1.0f;
  const float kMinY = 6.0f;
  const float kMaxY = 6.0f + static_cast<float>(map_->height()) - 1.0f;

  // 2. 타겟팅 매니저
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

  // 3. 가두기 로직
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

  // 4. 턴 매니지먼트 로직
  mint::ITurnActor* current = turn_manager_->current_actor();

  if (current == nullptr || current->IsActionFinished()) {
    // 턴 전환 수행 (NextTurn 내부에서 OnTurnEnd/OnTurnBegin이 자동 호출됨!)
    current = turn_manager_->NextTurn();

    if (current == nullptr) {
      MINT_LOG_INFO(L"새로운 라운드로 전환합니다.");
      turn_manager_->StartNewRound();
      current = turn_manager_->NextTurn();
    }
  }

  // 5. 엔진 기본 틱 호출
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

  ui_layout_->DrawBox(5, 5, map_->width() + 2, map_->height() + 2,
                      L"GRID ARENA", mint::Color::kRed);

  int info_y = 5 + map_->height() + 3;
  ui_layout_->DrawBox(5, info_y, 25, 6, L"내 파티 정보", mint::Color::kCyan);
  for (int i = 0; i < party_.size(); ++i) {
    if (party_[i] == nullptr) continue;
    renderer.Submit(party_[i]->name(), mint::Vector2(7, info_y + 1 + i),
                    mint::Color::kWhite);
    float hp_percent =
        static_cast<float>(party_[i]->hp()) / party_[i]->max_hp();
    mint::Color hp_color =
        (party_[i]->is_dead()) ? mint::Color::kRed : mint::Color::kGreen;
    ui_layout_->DrawProgressBar(14, info_y + 1 + i, 14, 1, hp_percent, L'■',
                                hp_color, mint::Color::kGray);
  }

  ui_layout_->DrawBox(35, info_y, 25, 6, L"적군 정보", mint::Color::kMagenta);
  for (int i = 0; i < enemies_.size(); ++i) {
    if (enemies_[i] == nullptr) continue;
    renderer.Submit(enemies_[i]->name(), mint::Vector2(37, info_y + 1 + i),
                    mint::Color::kWhite);
    float hp_percent =
        static_cast<float>(enemies_[i]->hp()) / enemies_[i]->max_hp();
    mint::Color hp_color =
        (enemies_[i]->is_dead()) ? mint::Color::kDarkGray : mint::Color::kRed;
    ui_layout_->DrawProgressBar(44, info_y + 1 + i, 14, 1, hp_percent, L'■',
                                hp_color, mint::Color::kGray);
  }

  // 결과 UI 출력
  if (status_ != BattleStatus::kInProgress) {
    int box_w = 30;
    int box_h = 7;
    int box_x = (map_->width() + 2 - box_w) / 2 + 5;
    int box_y = (map_->height() + 2 - box_h) / 2 + 5;

    if (status_ == BattleStatus::kVictory) {
      ui_layout_->DrawBox(box_x, box_y, box_w, box_h, L" BATTLE RESULT ",
                          mint::Color::kYellow, mint::Color::kBlack,
                          mint::Color::kYellow);
      ui_layout_->DrawTextAligned(box_x, box_y + 3, box_w, L"VICTORY!",
                                  mint::ui::Alignment::Center,
                                  mint::Color::kYellow);
    } else {
      ui_layout_->DrawBox(box_x, box_y, box_w, box_h, L" BATTLE RESULT ",
                          mint::Color::kRed, mint::Color::kBlack,
                          mint::Color::kRed);
      ui_layout_->DrawTextAligned(box_x, box_y + 3, box_w, L"DEFEAT...",
                                  mint::ui::Alignment::Center,
                                  mint::Color::kRed);
    }
    ui_layout_->DrawTextAligned(box_x, box_y + 5, box_w, L"Returning to Guild...",
                                mint::ui::Alignment::Center, mint::Color::kGray);
  }
}
}  // namespace guild
