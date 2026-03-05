#ifndef GUILD_MANAGER_ACTOR_ENEMY_H_
#define GUILD_MANAGER_ACTOR_ENEMY_H_

#include <string>

#include "MintEngine/Actor/Actor.h"

namespace guild {

/**
 * @brief 영웅들이 싸워야 할 적군 클래스
 */
class Enemy : public mint::Actor {
  RTTI_DECLARATIONS(Enemy, mint::Actor)

 public:
  /**
   * @brief Enemy 생성자
   * @param name 적의 이름
   * @param hp 최대 체력
   * @param atk 공격력
   * @param speed 속도
   * @param image 화면에 표시될 문자 (기본값 L"E")
   */
  Enemy(const std::wstring& name, int hp, int atk, int speed,
        const std::wstring& image = L"E");

  virtual ~Enemy() = default;

  void Tick(float delta_time) override;

  // Getter
  inline const std::wstring& name() const { return name_; }
  inline int hp() const { return hp_; }
  inline int max_hp() const { return max_hp_; }
  inline int atk() const { return atk_; }
  inline bool is_dead() const { return is_dead_; }

  /**
   * @brief 피해를 입는 함수
   * @param damage 입을 피해량
   */
  void TakeDamage(int damage);

  /**
   * @brief 상대방을 공격하는 함수
   * @param target 공격 대상
   */
  void Attack(mint::Actor* target);

  /**
   * @brief 공격 대상을 설정하는 함수
   * @param target 목표물 (Actor)
   */
  inline void set_target(mint::Actor* target) { target_ = target; }

 private:
  /**
   * @brief 타겟 방향으로 이동하는 함수
   * @param target_pos 목표 위치
   * @param delta_time 프레임 간 경과 시간
   */
  void MoveTowards(const mint::Vector2& target_pos, float delta_time);

  std::wstring name_;  ///< 적 이름
  int hp_;             ///< 현재 체력
  int max_hp_;         ///< 최대 체력
  int atk_;            ///< 공격력
  int speed_;          ///< 속도
  bool is_dead_ = false;       ///< 사망 판정

  mint::Actor* target_ = nullptr;  ///< 공격 대상
  float attack_timer_ = 0.0f;      ///< 공격 쿨타임용 타이머
};

}  // namespace guild

#endif  // GUILD_MANAGER_ACTOR_ENEMY_H_
