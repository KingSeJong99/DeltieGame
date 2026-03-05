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
  arena_info_ = {0, 0, 0, false};
}

void Hero::Tick(float delta_time) {
  if (is_dead()) return;

  if (!target_) return;

  Enemy* enemy_target = dynamic_cast<Enemy*>(target_);
  if (enemy_target && enemy_target->is_dead()) {
    target_ = nullptr;
    return;
  }

  mint::Vector2 target_pos = target_->position();
  mint::Vector2 my_pos = position();
  
  // 격자 좌표 차이 계산 (정수 단위로 비교)
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
    // 2. 사거리 밖이면 이동 쿨타임 체크 (이동도 속도에 영향받게 함)
    static float move_timer = 0.0f;
    move_timer += delta_time;
    float move_delay = 1.0f / (speed_ > 0 ? speed_ : 1);

    if (move_timer >= move_delay) {
      move_timer = 0.0f;
      MoveTowards(target_pos, delta_time);
    }
  }
}

void Hero::MoveTowards(const mint::Vector2& target_pos, float delta_time) {
  mint::Vector2 my_pos = position();
  
  // 격자 한 칸씩 툭툭 이동
  if (target_pos.x > my_pos.x) my_pos.x += 1.0f;
  else if (target_pos.x < my_pos.x) my_pos.x -= 1.0f;
  
  // @Todo: Y축 이동 추가 (지금은 X축 위주 전투니까)
  
  set_position(my_pos);
}

void Hero::TakeDamage(int damage) {
  if (is_dead()) return;

  hp_ -= damage;
  
  // 사망처리
  if (hp_ <= 0) {
    hp_ = 0;
    arena_info_.is_dead = true;
    arena_info_.death_count++;
  }
}

void Hero::Attack(mint::Actor* target) {
  if (is_dead() || !target) return;

  // 실제 타겟에게 대미지 가하기
  Enemy* enemy = dynamic_cast<Enemy*>(target);
  if (enemy) {
    enemy->TakeDamage(atk_);
    arena_info_.attack_point += atk_;
    
    if (enemy->is_dead()) {
      arena_info_.kill_count++;
      target_ = nullptr;
    }
  }
}

}  // namespace guild

