#ifndef MINT_ENGINE_CORE_TURN_MANAGER_H_
#define MINT_ENGINE_CORE_TURN_MANAGER_H_

#include <algorithm>
#include <queue>
#include <string>
#include <vector>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @interface ITurnActor
 * @brief 턴 시스템에 참여하는 모든 객체가 반드시 구현해야 할 인터페이스.
 */
class MINT_API ITurnActor {
 public:
  virtual ~ITurnActor() = default;

  /** @brief 턴이 시작될 때 호출되는 초기화 이벤트 함수. */
  virtual void OnTurnBegin() = 0;

  /** @brief 턴이 종료될 때 호출되는 정리 이벤트 함수. */
  virtual void OnTurnEnd() = 0;

  /** @brief 객체의 식별 이름을 반환한다. (디버깅 및 로그 출력용) */
  virtual std::wstring name() const = 0;

  /** @brief 객체의 턴 우선 순위를 결정하는 속도 값을 반환한다. */
  virtual int turn_speed() const = 0;

  /** @brief 현재 턴에서의 행동(이동, 공격 등)이 완료되었는지 여부를 반환한다.
   */
  virtual bool IsActionFinished() const = 0;
};

/**
 * @class TurnManager
 * @brief 라운드 기반의 전투 턴 순서를 관리하고 제어하는 매니저 클래스.
 */
class MINT_API TurnManager {
 public:
  TurnManager() = default;
  ~TurnManager() = default;

  /**
   * @brief 전투에 참여할 액터를 등록한다.
   * @param actor 턴 시스템에 참여할 ITurnActor 포인터.
   * @note 기물들을 participants_에 넣음
   */
  void AddParticipant(ITurnActor* actor);

  /**
   * @brief 새로운 라운드를 시작하며, 참여자들을 속도 순으로 정렬하여 턴 큐를
   * 생성한다.
   * @note 기존 큐를 비우고 participants_를 기준으로 turn_queue_에 다시
   * 정렬한다.
   */
  void StartNewRound();

  /**
   * @brief 현재 턴을 종료하고 다음 행동 순서의 액터를 활성화한다.
   * @return ITurnAcor* 새로 활성화된 액터의 포인터. 큐가 비어있으면 nullptr을
   * 반환한다.
   * @note 반환값이 nullptr인 경우 라운드가 종료된 것으로 간주한다.
   * turn_queue에 있는 기물들을 행동시키고 큐에서 제거한다.
   */
  ITurnActor* NextTurn();

  /** @brief 현재 행동 중인 액터의 포인터를 반환한다. */
  inline ITurnActor* current_actor() const { return current_actor_; }
  
  /** @brief 현재 라운드에 행동할 액터가 남아있는지 여부를 반환한다.*/
  inline bool IsRoundFinished() const { return turn_queue_.empty(); }

 private:
  std::vector<ITurnActor*>
      participants_;  ///< 라운드 시작 시 정렬 대상이 되는 전체 기물 목록
  std::queue<ITurnActor*> turn_queue_;   ///< 현재 라운드에서 행동 대기 중인 기물들의 순차 큐
  ITurnActor* current_actor_ = nullptr;  ///< 현재 턴의 주도권을 가진 기물에 대한 포인터
};

}  // namespace mint

#endif  // MINT_ENGINE_CORE_TURN_MANAGER_H_
