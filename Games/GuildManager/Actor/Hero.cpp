#include "Hero.h"
#include "Enemy.h"
#include "MintEngine/Level/GridMap.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Util/Pathfinder.h"

namespace guild {

Hero::Hero(const std::wstring& name, int hp, int atk, int speed, char grade,
           const std::wstring& image)
    : mint::Actor(image),
      name_(name),
      hp_(hp),
      max_hp_(hp),
      atk_(atk),
      speed_(speed),
      grade_(grade) {
  arena_info_ = {0, 0, 0, false, false, false, 0, CharacterState::kWaiting};
}

void Hero::Tick(float delta_time) {
  if (is_dead() || !arena_info_.is_my_turn || arena_info_.is_action_finished) return;

  if (!target_) {
    arena_info_.is_action_finished = true;
    return;
  }

  Enemy* enemy_target = dynamic_cast<Enemy*>(target_);
  if (enemy_target && enemy_target->is_dead()) {
    target_ = nullptr;
    arena_info_.is_action_finished = true;
    return;
  }

  // 격자 좌표 기준 거리 계산
  mint::IntVector2 my_grid = map_->WorldToGrid(position());
  mint::IntVector2 target_grid = map_->WorldToGrid(target_->position());
  
  int dist = std::abs(my_grid.x - target_grid.x) + std::abs(my_grid.y - target_grid.y);

  switch (arena_info_.state) {
    case CharacterState::kDeciding:
      // 사거리(1칸) 안에 있으면 공격
      if (dist <= 1) {
        arena_info_.state = CharacterState::kAttacking;
      } else {
        // 사거리 밖이고 이동력이 남았으며 경로가 있으면 이동
        if (arena_info_.current_move_points > 0 && !current_path_.empty()) {
          arena_info_.state = CharacterState::kMoving;
        } else {
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

void Hero::MoveTowardsNextNode() {
  if (current_path_.empty()) return;

  // 경로의 첫 번째 노드(현재 내 위치일 확률이 높음)는 건너뛰고 다음 노드로
  // (Pathfinder 구현에 따라 시작점이 포함될 수도 안 될 수도 있음)
  mint::IntVector2 next_grid = current_path_.front();
  
  // 만약 현재 위치와 같다면 다음 칸을 꺼냄
  if (next_grid == map_->WorldToGrid(position()) && current_path_.size() > 1) {
      current_path_.erase(current_path_.begin());
      next_grid = current_path_.front();
  }

  // 월드 좌표로 변환 (UI 오프셋 6.0f 고려)
  mint::Vector2 next_world_pos = map_->GridToWorld(next_grid.x, next_grid.y);
  next_world_pos.x += 6.0f;
  next_world_pos.y += 6.0f;

  set_position(next_world_pos);
  current_path_.erase(current_path_.begin());
}

void Hero::OnTurnBegin() {
  if (is_dead()) {
    arena_info_.is_action_finished = true;
    return;
  }

  arena_info_.is_my_turn = true;
  arena_info_.is_action_finished = false;
  arena_info_.state = CharacterState::kDeciding;
  arena_info_.current_move_points = speed_;
  action_timer_ = 0.0f;

  // 턴 시작 시 길 찾기 수행!
  if (map_ && target_) {
    mint::IntVector2 start = map_->WorldToGrid(position());
    mint::IntVector2 end = map_->WorldToGrid(target_->position());
    current_path_ = mint::Pathfinder::FindPath(map_, start, end);
    
    // 마지막 칸(적군이 서 있는 칸)은 경로에서 제외
    if (!current_path_.empty() && current_path_.back() == end) {
        current_path_.pop_back();
    }
  }
}

void Hero::OnTurnEnd() {
  arena_info_.is_my_turn = false;
  arena_info_.state = CharacterState::kWaiting;
  current_path_.clear();
}

bool Hero::IsActionFinished() const {
  return is_dead() || arena_info_.is_action_finished;
}

void Hero::TakeDamage(int damage) {
  if (is_dead()) return;
  hp_ -= damage;
  if (hp_ <= 0) {
    hp_ = 0;
    arena_info_.is_dead = true;
    arena_info_.death_count++;
  }
}

void Hero::Attack(mint::Actor* target) {
  if (is_dead() || !target) return;
  Enemy* enemy = dynamic_cast<Enemy*>(target);
  if (enemy && !enemy->is_dead()) {
    enemy->TakeDamage(atk_);
    arena_info_.attack_point += atk_;
    if (enemy->is_dead()) {
      arena_info_.kill_count++;
      target_ = nullptr;
    }
  }
}

}  // namespace guild
