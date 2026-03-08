#include "Enemy.h"

#include "Hero.h"
#include "MintEngine/Core/Logger.h"
#include "MintEngine/Render/Renderer.h"
#include "MintEngine/Level/GridMap.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Util/Pathfinder.h"

namespace guild {

Enemy::Enemy(const std::wstring& name, int hp, int atk, int speed,
             const std::wstring& image)
    : mint::Actor(name, image), hp_(hp), max_hp_(hp), atk_(atk), speed_(speed) {
  arena_info_ = {0, 0, 0, false, false, false, 0, CharacterState::kWaiting};
}

void Enemy::Draw(mint::Renderer& renderer, int width, int height) {
  mint::Color bg_color = mint::Color::kBlack;
  if (map_) {
    mint::IntVector2 grid_pos = map_->WorldToGrid(position());
    if (map_->IsWithinBounds(grid_pos.x, grid_pos.y)) {
      bg_color = map_->tile(grid_pos.x, grid_pos.y).is_walkable ? 
                 mint::Color::kBrightYellow : mint::Color::kDarkGray;
    }
  }

  if (is_dead()) {
    renderer.Submit(image_, position(), mint::Color::kDarkGray, bg_color, sorting_order_);
  } else {
    renderer.Submit(image_, position(), color_, bg_color, sorting_order_);
  }
}

void Enemy::Tick(float delta_time) {
  if (is_dead() || !arena_info_.is_my_turn || arena_info_.is_action_finished)
    return;

  if (!target_) {
    MINT_LOG_DEBUG_TAG(name(), L"목표가 없어 행동을 종료합니다.");
    arena_info_.is_action_finished = true;
    return;
  }

  Hero* hero_target = dynamic_cast<Hero*>(target_);
  if (hero_target && hero_target->is_dead()) {
    MINT_LOG_DEBUG_TAG(name(), L"새로운 대상을 찾는 중...");
    target_ = nullptr;
    arena_info_.is_action_finished = true;
    return;
  }

  // 격자 좌표 기준 거리 계산
  mint::IntVector2 my_grid = map_->WorldToGrid(position());
  mint::IntVector2 target_grid = map_->WorldToGrid(target_->position());

  int dist =
      std::abs(my_grid.x - target_grid.x) + std::abs(my_grid.y - target_grid.y);

  switch (arena_info_.state) {
    case CharacterState::kDeciding:
      // 사거리(1칸) 안에 있으면 공격
      if (dist <= 1) {
        MINT_LOG_DEBUG_TAG(name(), target_->name() + L"을 향해 공격합니다.");
        arena_info_.state = CharacterState::kAttacking;
      } else {
        // 사거리 밖이고 행동력이 남았으며 경로가 있으면 이동
        if (arena_info_.current_move_points > 0 && !current_path_.empty()) {
          MINT_LOG_DEBUG_TAG(name(), target_->name() + L"을 향해 이동합니다.");
          arena_info_.state = CharacterState::kMoving;
        } else {
          MINT_LOG_DEBUG_TAG(name(), L"행동력을 모두 소진하여 턴을 마칩니다.");
          arena_info_.is_action_finished = true;
        }
      }
      action_timer_ = 0.0f;
      break;

    case CharacterState::kMoving:
      action_timer_ += delta_time;
      if (action_timer_ >= 0.2f) {
        MoveTowardsNextNode();
        arena_info_.current_move_points--;
        action_timer_ = 0.0f;
        arena_info_.state = CharacterState::kDeciding;
      }
      break;

    case CharacterState::kAttacking:
      action_timer_ += delta_time;
      if (action_timer_ >= 0.5f) {
        Attack(target_);
        arena_info_.is_action_finished = true;
      }
      break;

    default:
      break;
  }
}

void Enemy::OnTurnBegin() {
  if (is_dead()) {
    arena_info_.is_action_finished = true;
    return;
  }

  arena_info_.is_my_turn = true;
  arena_info_.is_action_finished = false;
  arena_info_.state = CharacterState::kDeciding;
  arena_info_.current_move_points = speed_;
  action_timer_ = 0.0f;

  MINT_LOG_INFO_TAG(name(), L"턴 시작! 행동력: " + std::to_wstring(speed_));

  // 턴 시작 시 길 찾기 수행!
  if (map_ && target_) {
    mint::IntVector2 start = map_->WorldToGrid(position());
    mint::IntVector2 end = map_->WorldToGrid(target_->position());
    current_path_ = mint::Pathfinder::FindPath(map_, start, end);

    // 경로의 첫 번째 칸은 현재 내 위치이므로 미리 제거
    if (!current_path_.empty() && current_path_.front() == start) {
      current_path_.erase(current_path_.begin());
    }

    // 마지막 칸(적군이 서 있는 칸)은 경로에서 제외
    if (!current_path_.empty() && current_path_.back() == end) {
      current_path_.pop_back();
    }
  }
}

void Enemy::OnTurnEnd() {
  arena_info_.is_my_turn = false;
  arena_info_.state = CharacterState::kWaiting;
  current_path_.clear();
}

bool Enemy::IsActionFinished() const {
  return is_dead() || arena_info_.is_action_finished;
}

void Enemy::MoveTowardsNextNode() {
  if (current_path_.empty()) return;

  // 다음 이동할 목적지 노드를 꺼냄
  mint::IntVector2 next_grid = current_path_.front();
  current_path_.erase(current_path_.begin());

  // 월드 좌표로 변환 (UI 오프셋 6.0f 고려)
  mint::Vector2 next_world_pos = map_->GridToWorld(next_grid.x, next_grid.y);
  next_world_pos.x += 6.0f;
  next_world_pos.y += 6.0f;

  set_position(next_world_pos);
}

void Enemy::TakeDamage(int damage) {
  if (is_dead()) return;
  hp_ -= damage;
  if (hp_ <= 0) {
    hp_ = 0;
    arena_info_.is_dead = true;
    arena_info_.death_count++;
  }
}

void Enemy::Attack(mint::Actor* target) {
  if (is_dead() || !target) return;
  Hero* hero = dynamic_cast<Hero*>(target);
  if (hero && !hero->is_dead()) {
    hero->TakeDamage(atk_);
    arena_info_.attack_point += atk_;

    if (hero->is_dead()) {
      arena_info_.kill_count++;
      target_ = nullptr;
    }
  }
}

}  // namespace guild
