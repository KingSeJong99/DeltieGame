#ifndef GUILD_MANAGER_ACTOR_HERO_H_
#define GUILD_MANAGER_ACTOR_HERO_H_

#include <string>

#include "MintEngine/Actor/Actor.h"
#include "MintEngine/Core/TurnManager.h"
#include "Common/GameTypes.h"

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
    int current_move_points = 0;      ///< 남은 이동 가능 횟수
    CharacterState state = CharacterState::kWaiting;
  };

  Hero(const std::wstring& name, int hp, int atk, int speed, char grade,
       const std::wstring& image = L"H");

  virtual ~Hero() = default;

  void Tick(float delta_time) override;

  // ITurnActor Overrides
  inline int turn_speed() const override { return speed_; }
  bool IsActionFinished() const override;
  void OnTurnBegin() override;
  void OnTurnEnd() override;

  // Getter
  inline struct ArenaInfo arena_info() const { return arena_info_; }
  inline const std::wstring& name() const { return name_; }
  inline int hp() const { return hp_; }
  inline int max_hp() const { return max_hp_; }
  inline int atk() const { return atk_; }
  inline bool is_dead() const { return arena_info_.is_dead; }

  void TakeDamage(int damage);
  void Attack(mint::Actor* target);
  inline void set_target(mint::Actor* target) { target_ = target; }

 private:
  void MoveTowards(const mint::Vector2& target_pos);

  struct ArenaInfo arena_info_;
  std::wstring name_;
  int hp_;
  int max_hp_;
  int atk_;
  int speed_;
  char grade_;

  mint::Actor* target_ = nullptr;
  float action_timer_ = 0.0f;
};

}  // namespace guild

#endif  // GUILD_MANAGER_ACTTOR_HERO_H_
