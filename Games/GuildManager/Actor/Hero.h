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
}

namespace guild {

class Hero : public mint::Actor, public mint::ITurnActor {
  RTTI_DECLARATIONS(Hero, mint::Actor)

 public:
  struct ArenaInfo {
    int attack_point;
    int kill_count;
    int death_count;
    bool is_dead = false;
    bool is_my_turn = false;
    bool is_action_finished = false;
    int current_move_points = 0;
    CharacterState state = CharacterState::kWaiting;
  };

  Hero(const std::wstring& name, int hp, int atk, int speed, char grade,
       const std::wstring& image = L"H");

  virtual ~Hero() = default;

  void Tick(float delta_time) override;

  // ITurnActor Overrides
  inline std::wstring name() const override { return name_; }
  inline int turn_speed() const override { return speed_; }
  bool IsActionFinished() const override;
  void OnTurnBegin() override;
  void OnTurnEnd() override;

  // Getter & Setter
  inline struct ArenaInfo arena_info() const { return arena_info_; }
  inline mint::Actor* target() const { return target_; }
  inline int hp() const { return hp_; }
  inline int max_hp() const { return max_hp_; }
  inline int atk() const { return atk_; }
  inline bool is_dead() const { return arena_info_.is_dead; }
  void set_grid_map(mint::GridMap* map) { map_ = map; }
  inline void set_target(mint::Actor* target) { target_ = target; }

  void TakeDamage(int damage);
  void Attack(mint::Actor* target);

 private:
  void MoveTowardsNextNode();

  struct ArenaInfo arena_info_;
  std::wstring name_;
  int hp_;
  int max_hp_;
  int atk_;
  int speed_;
  char grade_;

  mint::Actor* target_ = nullptr;
  mint::GridMap* map_ = nullptr;               ///< 전장 지도
  std::vector<mint::IntVector2> current_path_; ///< 찾아놓은 경로
  float action_timer_ = 0.0f;
};

}  // namespace guild

#endif  // GUILD_MANAGER_ACTTOR_HERO_H_
