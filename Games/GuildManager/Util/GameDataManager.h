#pragma once
#ifndef GUILD_MANAGER_UTIL_GAME_DATA_MANAGER_H_
#define GUILD_MANAGER_UTIL_GAME_DATA_MANAGER_H_

#include <algorithm>
#include <string>
#include <vector>

#include "Common/GameTypes.h"

namespace guild {

class Hero;

/**
 * @class GameDataManager
 * @brief 게임의 전역 데이터(자원, 용사 목록 등)를 관리하는 싱글톤 클래스.
 * @details 레벨 전환 시에도 유지되어야 하는 모든 영속 데이터를 소유한다.
 * @note @Todo: 데이터 파일을 만들어서 저장하는 방법을 생각중.
 */
class GameDataManager {
 public:
  /** @brief 싱글톤 인스턴스를 반환한다. */
  static GameDataManager& Get() {
    static GameDataManager instance;
    return instance;
  }

  // 복사 및 대입 금지 (싱글톤 원칙 준수)
  GameDataManager(const GameDataManager&) = delete;
  GameDataManager& operator=(const GameDataManager&) = delete;

  // --- Accessors (Getter / Setter) ---
  int money() const { return money_; }
  void AddMoney(int amount) { money_ += amount; }

  bool SpendMoney(int amount) {
    if (money_ < amount) return false;
    money_ -= amount;
    return true;
  }

  std::vector<Hero*>& owned_heroes() { return owned_heroes_; }
  std::vector<Hero*>& party() { return party_; }

  /** @brief 가챠로 얻은 용사를 리스트에 추가한다. */
  void AddHero(Hero* hero) {
    if (hero) owned_heroes_.push_back(hero);
  }

  bool IsInParty(Hero* hero) const {
    for (auto* p : party_)
      if (p == hero) return true;
    return false;
  }

 private:
  // 시작 자금 10,000M로 초기화
  GameDataManager() : money_(10000) {}
  ~GameDataManager() = default;

  int money_;                        ///< 현재 소지금
  std::vector<Hero*> owned_heroes_;  ///< 전체 보유 용사 목록
  std::vector<Hero*> party_;         ///< 현재 편성된 파티 (최대 3명)
};

}  // namespace guild

#endif  // GUILD_MANAGER_UTIL_GAME_DATA_MANAGER_H_