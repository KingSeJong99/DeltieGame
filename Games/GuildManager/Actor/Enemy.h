#ifndef GUILD_MANAGER_ACTOR_ENEMY_H_
#define GUILD_MANAGER_ACTOR_ENEMY_H_

#include <string>

#include "Common/GameTypes.h"
#include "MintEngine/Actor/Actor.h"
#include "MintEngine/Core/TurnManager.h"

namespace guild {

class Enemy : public mint::Actor, public mint::ITurnActor {
  RTTI_DECLARATIONS(Enemy, mint::Actor)

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

  Enemy(const std::wstring& name, int hp, int atk, int speed,
        const std::wstring& image = L"E");

  virtual ~Enemy() = default;

  void Tick(float delta_time) override;

  // 인터페이스 오버라이딩
  inline std::wstring name() const override { return name_; }
  inline int turn_speed() const override { return speed_; }
  bool IsActionFinished() const override;
  void OnTurnBegin() override;
  void OnTurnEnd() override;

  // Getter
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

  mint::Actor* target_ = nullptr;
  float action_timer_ = 0.0f;
};

}  // namespace guild

#endif  // GUILD_MANAGER_ACTOR_ENEMY_H_
