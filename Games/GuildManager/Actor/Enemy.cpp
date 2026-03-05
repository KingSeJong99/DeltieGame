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

  // 1. 사거리 체크
  if (abs(distance_x) <= 1.5f) {
    attack_timer_ += delta_time;
    float attack_speed = 2.0f / (speed_ > 0 ? speed_ : 1);

    if (attack_timer_ >= attack_speed) {
      attack_timer_ = 0.0f;
      Attack(target_);
    }
  } else {
    // 2. 사거리 밖이면 이동
    MoveTowards(target_pos, delta_time);
  }
}

void Enemy::MoveTowards(const mint::Vector2& target_pos, float delta_time) {
  mint::Vector2 my_pos = position();
  float direction = (target_pos.x > my_pos.x) ? 1.0f : -1.0f;

  // 적 이동 속도: Hero보다는 살짝 느리게 (1.5 + speed * 0.15)
  float move_velocity = 1.5f + (static_cast<float>(speed_) * 0.15f);
  
  my_pos.x += direction * move_velocity * delta_time;
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
