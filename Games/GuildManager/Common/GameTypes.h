#ifndef GUILD_MANAGER_COMMON_GAME_TYPES_H_
#define GUILD_MANAGER_COMMON_GAME_TYPES_H_

namespace guild {

/**
 * @brief 캐릭터의 현재 행동 상태를 정의하는 열거형
 */
enum class CharacterState {
  kWaiting = 0, ///< 턴 대기 중
  kDeciding,    ///< 행동 결정 중
  kMoving,      ///< 이동 중
  kAttacking,   ///< 공격 중
};

}  // namespace guild

#endif  // GUILD_MANAGER_COMMON_GAME_TYPES_H_
