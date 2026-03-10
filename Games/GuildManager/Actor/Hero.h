#ifndef GUILD_MANAGER_ACTOR_HERO_H_
#define GUILD_MANAGER_ACTOR_HERO_H_

#include "CombatUnit.h"

namespace guild {

/**
 * @class Hero
 * @brief 플레이어가 관리하는 용사 클래스.
 * @note CombatUnit을 상속받아 전투 기능을 수행한다.
 */
class Hero : public CombatUnit {
  RTTI_DECLARATIONS(Hero, CombatUnit)

 public:
  Hero(const std::wstring& name, CharacterJob job, int hp, int atk, int speed, char grade,
       const std::wstring& image = L"H");

  virtual ~Hero() = default;

  void Attack(mint::Actor* target) override;
};

}  // namespace guild

#endif  // GUILD_MANAGER_ACTOR_HERO_H_
