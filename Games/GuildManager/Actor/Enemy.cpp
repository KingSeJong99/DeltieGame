#include "Enemy.h"
#include "Hero.h"
#include "MintEngine/Math/Vector2.h"

namespace guild {

Enemy::Enemy(const std::wstring& name, int hp, int atk, int speed,
             const std::wstring& image)
    : mint::Actor(image),
      name_(name),
      hp_(hp),
      max_hp_(hp),
      atk_(atk),
      speed_(speed) {
  arena_info_ = {0, 0, 0, false, false, false, 0, CharacterState::kWaiting};
}

void Enemy::Tick(float delta_time) {
  if (is_dead() || !arena_info_.is_my_turn || arena_info_.is_action_finished) return;

  if (!target_) {
    arena_info_.is_action_finished = true;
    return;
  }

  Hero* hero_target = dynamic_cast<Hero*>(target_);
  if (hero_target && hero_target->is_dead()) {
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
      if (action_timer_ >= 0.3f) {
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

void Enemy::MoveTowards(const mint::Vector2& target_pos) {
  mint::Vector2 my_pos = position();
  if (target_pos.x > my_pos.x)
    my_pos.x += 1.0f;
  else if (target_pos.x < my_pos.x)
    my_pos.x -= 1.0f;
  set_position(my_pos);
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
}

void Enemy::OnTurnEnd() {
  arena_info_.is_my_turn = false;
  arena_info_.state = CharacterState::kWaiting;
}

bool Enemy::IsActionFinished() const {
  return is_dead() || arena_info_.is_action_finished;
}

void Enemy::TakeDamage(int damage) {
  if (is_dead()) return;
  hp_ -= damage;
  if (hp_ <= 0) {
    hp_ = 0;
    arena_info_.is_dead = true;
  }
}

void Enemy::Attack(mint::Actor* target) {
  if (is_dead() || !target) return;
  Hero* hero = dynamic_cast<Hero*>(target);
  if (hero && !hero->is_dead()) {
    hero->TakeDamage(atk_);
  }
}

}  // namespace guild
