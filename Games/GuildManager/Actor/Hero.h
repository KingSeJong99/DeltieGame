#ifndef GUILD_MANAGER_ACTOR_HERO_H_
#define GUILD_MANAGER_ACTOR_HERO_H_

#include <string>
#include <vector>

#include "MintEngine/Actor/Actor.h"
#include "MintEngine/Core/TurnManager.h"
#include "MintEngine/Math/IntVector2.h"
#include "Common/GameTypes.h"

namespace mint {
class GridMap;
class Renderer;
}

namespace guild {

    /**
     * @class Hero
     * @brief 플레이어가 관리하고 전투에 참여시키는 용사 클래스
     * @note Actor를 상속받아 시각적 표현과 위치를 관리하며, ITurnActor 인터페이스를
     * 구현하여 턴 시스템에 참여한다
     */
class Hero : public mint::Actor, public mint::ITurnActor {
  RTTI_DECLARATIONS(Hero, mint::Actor)

 public:
  /**
   * @struct ArenaInfo
   * @brief 전투(Arena) 도중의 상태 및 통계 정보를 저장하는 구조체.
   * @note 행동력 만큼 이동을 할 수 있지만, 공격을 하고 나면 남은 행동력에
   * 상관없이 자신의 턴을 끝내야한다.
   */
  struct ArenaInfo {
    int attack_point = 0;                   ///< 이번 전투에서 가한 누적 대미지
    int kill_count = 0;                     ///< 적을 처치한 횟수
    int death_count = 0;                    ///< 사망한 횟수
    bool is_dead = false;               ///< 현재 사망 상태 여부
    bool is_my_turn = false;            ///< 현재 자신의 턴인가를 판단하는 여부
    bool is_action_finished = false;    ///< 이번 턴의 행동(이동/공격) 완료 여부
    int current_move_points = 0;        ///< 이번 턴에 남은 행동력
    CharacterState state = CharacterState::kWaiting;    ///< 현재 행동 상태
  };

  /**
   * @brief Hero 생성자.
   * @param name 용사의 이름.
   * @param hp 최대 체력.
   * @param atk 공격력.
   * @param speed 행동 속도(턴 순서 결정).
   * @param grade 용사의 등급 (S, A, B, C 등).
   * @param image 화면에 표시될 문자열 (기본값 "H").
   */
  Hero(const std::wstring& name, int hp, int atk, int speed, char grade,
       const std::wstring& image = L"H");

  virtual ~Hero() = default;

  /** @brief 매 프레임 용사의 AI 로직 및 상태를 업데이트 한다. */
  void Tick(float delta_time) override;
  
  /** @brief 용사의 상태(사망 여부 등)를 반영하여 화면에 그린다. */
  void Draw(mint::Renderer& renderer, int width, int height) override;

  // --- ITurnActor 인터페이스 구현 ---
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

  // --- Getter & Setter ---
  inline struct ArenaInfo arena_info() const { return arena_info_; }
  inline mint::Actor* target() const { return target_; }
  inline int hp() const { return hp_; }
  inline int max_hp() const { return max_hp_; }
  inline int atk() const { return atk_; }
  inline bool is_dead() const { return arena_info_.is_dead; }

  /** @brief 전장 맵 포인터를 설정한다. */
  void set_grid_map(mint::GridMap* map) { map_ = map; }
  /** @brief 공격 대상(Actor)을 설정한다. */
  inline void set_target(mint::Actor* target) { target_ = target; }

  /** @brief 지정된 대미지만큼 체력을 감소시킨다. */
  void TakeDamage(int damage);
  /** @brief 대상 액터를 공격하여 대미지를 가한다. */
  void Attack(mint::Actor* target);

 private:
  /** @brief 다음 경로 노드로 한 칸 이동하고 월드 좌표를 업데이트한다. */
  void MoveTowardsNextNode();

  struct ArenaInfo arena_info_;                ///< 전투 관련 세부 상태 정보
  int hp_;                                     ///< 현재 체력
  int max_hp_;                                 ///< 최대 체력
  int atk_;                                    ///< 공격력
  int speed_;                                  ///< 행동 속도
  char grade_;                                 ///< 용사 등급

  mint::Actor* target_ = nullptr;              ///< 현재 추적 또는 공격 중인 대상
  mint::GridMap* map_ = nullptr;               ///< 현재 속해 있는 전장의 그리드 맵
  std::vector<mint::IntVector2> current_path_; ///< 목표까지의 이동 경로
  float action_timer_ = 0.0f;                  ///< 애니메이션 또는 행동 지연용 타이머
};

}  // namespace guild

#endif  // GUILD_MANAGER_ACTOR_HERO_H_
