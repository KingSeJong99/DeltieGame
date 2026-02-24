#ifndef GUILD_MANAGER_ACTOR_HERO_H_
#define GUILD_MANAGER_ACTOR_HERO_H_

#include <string>

#include "MintEngine/Actor/Actor.h"

namespace guild {

/**
 * @brief 길드 매니저의 핵심 객체인 용사 클래스
 */
class Hero : public mint::Actor {
  RTTI_DECLARATIONS(Hero, mint::Actor)

 public:
  /**
   * @brief 용사가 가진 전투 정보를 저장하는 구조체
   */
  struct ArenaInfo {
    int attack_point;  ///< 해당 라운드에 입힌 총 피해량
    int kill_count;    ///< 처치한 적의 수
    int death_count;   ///< 사망 횟수
    bool is_dead = false;   ///< 사망 판정
  };

  /**
   * @brief Hero 생성자
   * @param name 용사의 이름
   * @param hp 최대 체력
   * @param atk 공격력
   * @param speed 속도
   * @param grade 등급 (S, A, B, C)
   * @param image 화면에 표시될 문자 (기본값 L"H")
   */
  Hero(const std::wstring& name, int hp, int atk, int speed, char grade,
       const std::wstring& image = L"H");

  virtual ~Hero() = default;

  /**
   * @brief 매 프레임 업데이트 되는 로직
   * @param delta_time 프레임 간 경과 시간
   */
  void Tick(float delta_time) override;

  // Getter 
  inline struct ArenaInfo arena_info() const { return arena_info_; }
  inline const std::wstring& name() const { return name_; }
  inline int hp() const { return hp_; }
  inline int max_hp() const { return max_hp_; }
  inline int atk() const { return atk_; }
  inline bool is_dead() const { return arena_info_.is_dead;}

  /**
   * @brief 피해를 입는 함수
   * @param damage 입을 피해량
   */
  void TakeDamage(int damage);

  /**
   * @brief 피해를 입히는 함수
   * @param damage 입힐 피해량
   */

 private:

  struct ArenaInfo arena_info_;   ///< 전투 통계 정보
  std::wstring name_;               ///< 용사 이름
  int hp_;                          ///< 현재 체력
  int max_hp_;                      ///< 최대 체력
  int atk_;                         ///< 공격력
  int speed_;                       ///< 속도
  char grade_;                      ///< 등급
};

}  // namespace guild

#endif  // GUILD_MANAGER_ACTTOR_HERO_H_