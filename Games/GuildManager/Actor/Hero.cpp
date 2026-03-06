#include "Hero.h"
#include "Enemy.h"
#include "MintEngine/Math/Vector2.h"

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
  // 죽었거나 내 턴이 아니면 즉시 종료
  if (is_dead() || !arena_info_.is_my_turn || arena_info_.is_action_finished) return;

  // 타겟 유효성 검사
  if (!target_) {
    arena_info_.is_action_finished = true;
    return;
  }

  Enemy* enemy_target = dynamic_cast<Enemy*>(target_);
  if (enemy_target && enemy_target->is_dead()) {
    // 타겟이 이미 죽었으면 이번 턴은 더 이상 할 게 없음
    target_ = nullptr;
    arena_info_.is_action_finished = true;
    return;
  }

  mint::Vector2 target_pos = target_->position();
  mint::Vector2 my_pos = position();
  float distance_x = abs(target_pos.x - my_pos.x);

  switch (arena_info_.state) {
    case CharacterState::kDeciding:
      if (distance_x <= 1.1f) {
        arena_info_.state = CharacterState::kAttacking;
      } else {
        if (arena_info_.current_move_points > 0) {
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
        MoveTowards(target_pos);
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

void Hero::MoveTowards(const mint::Vector2& target_pos) {
  mint::Vector2 my_pos = position();
  if (target_pos.x > my_pos.x) my_pos.x += 1.0f;
  else if (target_pos.x < my_pos.x) my_pos.x -= 1.0f;
  set_position(my_pos);
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
}

void Hero::OnTurnEnd() {
  arena_info_.is_my_turn = false;
  arena_info_.state = CharacterState::kWaiting;
}

bool Hero::IsActionFinished() const {
  // 죽었으면 무조건 끝난 걸로 간주해서 TurnManager가 넘어가게 함
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
  if (enemy && !enemy->is_dead()) { // 죽은 놈은 또 패지 않는다!
    enemy->TakeDamage(atk_);
    arena_info_.attack_point += atk_;
    if (enemy->is_dead()) {
      arena_info_.kill_count++;
      target_ = nullptr;
    }
  }
}

}  // namespace guild
