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
      speed_(speed),
      is_dead_(false) {}

void Enemy::Tick(float delta_time) {
  if (is_dead()) return;

  if (!target_) return;

  Hero* hero_target = dynamic_cast<Hero*>(target_);
  if (hero_target && hero_target->is_dead()) {
    target_ = nullptr;
    return;
  }

  mint::Vector2 target_pos = target_->position();
  mint::Vector2 my_pos = position();
  float distance_x = target_pos.x - my_pos.x;

  // 1. 사거리 체크 (격자 1칸 이내)
  if (abs(distance_x) <= 1.1f) {
    attack_timer_ += delta_time;
    float attack_speed = 2.0f / (speed_ > 0 ? speed_ : 1);

    if (attack_timer_ >= attack_speed) {
      attack_timer_ = 0.0f;
      Attack(target_);
    }
  } else {
    // 2. 사거리 밖이면 이동 쿨타임 체크 (이동 속도는 영웅보다 살짝 느리게)
    static float move_timer = 0.0f;
    move_timer += delta_time;
    float move_delay = 1.2f / (speed_ > 0 ? speed_ : 1);

    if (move_timer >= move_delay) {
      move_timer = 0.0f;
      MoveTowards(target_pos, delta_time);
    }
  }
}

void Enemy::MoveTowards(const mint::Vector2& target_pos, float delta_time) {
  mint::Vector2 my_pos = position();
  
  // 격자 한 칸씩 툭툭 이동
  if (target_pos.x > my_pos.x) my_pos.x += 1.0f;
  else if (target_pos.x < my_pos.x) my_pos.x -= 1.0f;
  
  set_position(my_pos);
}

void Enemy::TakeDamage(int damage) {
  if (is_dead()) return;

  hp_ -= damage;
  if (hp_ <= 0) {
    hp_ = 0;
    is_dead_ = true;
  }
}

void Enemy::Attack(mint::Actor* target) {
  if (is_dead() || !target) return;

  Hero* hero = dynamic_cast<Hero*>(target);
  if (hero) {
    hero->TakeDamage(atk_);
  }
}

}  // namespace guild
