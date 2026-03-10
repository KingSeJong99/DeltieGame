#ifndef GUILD_MANAGER_ACTOR_COMBAT_UNIT_H_
#define GUILD_MANAGER_ACTOR_COMBAT_UNIT_H_

#include <string>
#include <vector>

#include "Common/GameTypes.h"
#include "MintEngine/Actor/Actor.h"
#include "MintEngine/Core/TurnManager.h"
#include "MintEngine/Math/IntVector2.h"

namespace mint {
class GridMap;
class Renderer;
}  // namespace mint

namespace guild {

class BattleLevel;

/**
 * @class CombatUnit
 * @brief 전투에 참여하는 모든 기물(Hero, Enemy)의 기본 클래스.
 * @note 공통적인 전투 데이터와 턴 제어 로직을 관리한다.
 */
class CombatUnit : public mint::Actor, public mint::ITurnActor {
  RTTI_DECLARATIONS(CombatUnit, mint::Actor)

 public:
  struct ArenaInfo {
    int attack_point = 0;             ///< 이번 전투에서 가한 누적 대미지
    int kill_count = 0;               ///< 적을 처치한 횟수
    int death_count = 0;              ///< 사망한 횟수
    bool is_dead = false;             ///< 현재 사망 상태 여부
    bool is_my_turn = false;          ///< 현재 자신의 턴인가를 판단하는 여부
    bool is_action_finished = false;  ///< 이번 턴의 행동(이동/공격) 완료 여부
    int current_move_points = 0;      ///< 이번 턴에 남은 행동력
    int max_move_points = 0;          ///< 이번에 추가한 현재/최대 MP용
    CharacterState state = CharacterState::kWaiting;  ///< 현재 행동 상태
  };

  /**
   * @brief CombatUnit 생성자.
   * @param name 기물의 이름.
   * @param job 기물의 직업.
   * @param hp 최대 체력.
   * @param atk 공격력.
   * @param speed 행동 속도(턴 순서 결정).
   * @param grade 용사의 등급 (S, A, B, C 등).
   * @param image 화면에 표시될 문자열 (기본값 "H").
   */
  CombatUnit(const std::wstring& name, CharacterJob job, int hp, int atk,
             int speed, char grade, const std::wstring& image);
  virtual ~CombatUnit() = default;

  // --- Actor/ITurnActor 인터페이스 ---
  /** @brief 매 프레임 용사의 AI 로직 및 상태를 업데이트 한다. */
  void Tick(float delta_time) override;
  /** @brief 용사의 상태(사망 여부 등)를 반영하여 화면에 그린다. */
  void Draw(mint::Renderer& renderer, int width, int height) override;

  /** @brief 부모 클래스 Actor의 이름을 반환한다. */
  inline std::wstring name() const override { return Actor::name(); }
  /** @brief 용사의 속도(speed_) 값을 턴 속도로 반환한다. */
  inline int turn_speed() const override { return speed_; }
  /** @brief 이번 턴의 행동이 완료되었는지 확인한다. */
  bool IsActionFinished() const override;
  /** @brief 턴이 시작될 때 상태를 초기화하고 길찾기를 수행한다. */
  void OnTurnBegin() override;
  /** @brief 턴이 종료될 때 상태를 정리한다. */
  void OnTurnEnd() override;

  // --- 공통 기능 ---
  /** @brief 지정된 대미지만큼 체력을 감소시킨다. */
  void TakeDamage(int damage);
  /** @brief 대상 액터를 공격하여 대미지를 가한다. */
  virtual void Attack(mint::Actor* target);

  // --- Getter/Setter ---
  inline const ArenaInfo& arena_info() const { return arena_info_; }
  inline int hp() const { return hp_; }
  inline int max_hp() const { return max_hp_; }
  inline int atk() const { return atk_; }
  inline bool is_dead() const { return arena_info_.is_dead; }
  inline CharacterJob job() const { return job_; }
  inline mint::Actor* target() const { return target_; }
  inline char grade() const { return grade_; }
  inline int speed() const { return speed_; }

  /** @brief 전장 맵 포인터를 설정한다. */
  void set_grid_map(mint::GridMap* map) { map_ = map; }
  /** @brief 전장 레벨 포인터를 설정한다. */
  void set_battle_level(BattleLevel* level) { battle_level_ = level; }
  /** @brief 공격 대상(Actor)을 설정한다. */
  inline void set_target(mint::Actor* target) { target_ = target; }

  /** @brief 현재 계획된 이동 경로를 반환한다. */
  inline const std::vector<mint::IntVector2>& current_path() const {
    return current_path_;
  }

 protected:
  /** @brief 다음 경로 노드로 한 칸 이동하고 월드 좌표를 업데이트한다. */
  void MoveTowardsNextNode();

  /** @brief 유닛의 행동력을 계산한다.*/
  int GetCalculateMovePoints() const;

  int hp_;                      ///< 현재 체력
  int max_hp_;                  ///< 최대 체력
  int atk_;                     ///< 공격력
  int speed_;                   ///< 행동 속도
  CharacterJob job_;            ///< 직업
  char grade_;                  ///< 용사 등급

  mint::Actor* target_ = nullptr;
  mint::GridMap* map_ = nullptr;
  BattleLevel* battle_level_ = nullptr; ///< 소속된 전투 레벨
  std::vector<mint::IntVector2> current_path_;
  float action_timer_ = 0.0f;
  std::wstring default_image_;     ///< 캐릭터 본래의 이미지 (H, E 등)
  ArenaInfo arena_info_;        ///< 전투 관련 세부 상태 정보
};

}  // namespace guild

#endif  // GUILD_MANAGER_ACTOR_COMBAT_UNIT_H_
