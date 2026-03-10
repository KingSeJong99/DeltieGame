#include "Hero.h"
#include "Enemy.h"
#include "Level/BattleLevel.h"
#include "MintEngine/Core/Logger.h"

namespace guild {

Hero::Hero(const std::wstring& name, CharacterJob job, int hp, int atk,
           int speed, char grade, const std::wstring& image)
    : CombatUnit(name, job, hp, atk, speed, grade, image) {}

void Hero::Attack(mint::Actor* target) {
  if (is_dead() || !target) return;

  CombatUnit* combat_target = dynamic_cast<CombatUnit*>(target);
  if (combat_target && !combat_target->is_dead()) {
    combat_target->TakeDamage(atk_);
    arena_info_.attack_point += atk_;
    
    if (battle_level_) {
      battle_level_->AddCombatLog(name() + L" -> " + combat_target->name() + L" (" + std::to_wstring(atk_) + L" DMG)");
    }

    MINT_LOG_INFO_TAG(name(), combat_target->name() + L"에게 " + std::to_wstring(atk_) + L"의 데미지를 입혔어요!");

    if (combat_target->is_dead()) {
      arena_info_.kill_count++;
      if (battle_level_) {
        battle_level_->AddCombatLog(combat_target->name() + L"이(가) 쓰러졌습니다!");
      }
      target_ = nullptr;
    }
  }
}

}  // namespace guild
