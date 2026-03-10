#ifndef GUILD_MANAGER_ACTOR_ENEMY_H_
#define GUILD_MANAGER_ACTOR_ENEMY_H_

#include "CombatUnit.h"

namespace guild {

/**
 * @class Enemy
 * @brief 용사를 공격하는 적 유닛 클래스.
 * @note CombatUnit을 상속받아 전투 기능을 수행한다.
 */
class Enemy : public CombatUnit {
  RTTI_DECLARATIONS(Enemy, CombatUnit)

 public:
  Enemy(const std::wstring& name, CharacterJob job, int hp, int atk, int speed,
        char grade, const std::wstring& image = L"E");

  virtual ~Enemy() = default;

  void Attack(mint::Actor* target) override;
};

}  // namespace guild

#endif  // GUILD_MANAGER_ACTOR_ENEMY_H_
