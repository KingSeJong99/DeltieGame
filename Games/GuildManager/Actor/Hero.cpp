#include "Hero.h"

guild::Hero::Hero(const std::wstring& name, int hp, int atk, int speed,
                  char grade, const std::wstring& image)
    : mint::Actor(image),
      name_(name),
      hp_(hp),
      atk_(atk),
      speed_(speed),
      grade_(grade) {}

void guild::Hero::Tick(float delta_time) {
    // 죽은 상태가 되면 로직 중단
  if (is_dead()) {
    return;
  }

  // Todo: 용사의 자율 행동 로직 추가 예정
}

void guild::Hero::TakeDamage(int damage) {
  if (is_dead()) {
    return;
  }

  // Todo: 상대의 공격력에 계산하여 피격받아야함
  hp_ -= damage;

  // 체력이 0 밑으로 내려가면 사망판정
  if (hp_ <= 0) {
    hp_ = 0;
    arena_info_.is_dead = true ;
    arena_info_.death_count++;
  }
}
