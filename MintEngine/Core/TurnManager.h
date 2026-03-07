#ifndef MINT_ENGINE_CORE_TURN_MANAGER_H_
#define MINT_ENGINE_CORE_TURN_MANAGER_H_

#include <vector>
#include <queue>
#include <algorithm>
#include <string>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 턴 시스템에 참여하는 객체들이 구현해야 할 인터페이스
 */
class MINT_API ITurnActor {
 public:
  virtual ~ITurnActor() = default;

  /**
   * @brief 내 턴이 시작되었을 때 호출
   */
  virtual void OnTurnBegin() = 0;

  /**
   * @brief 내 턴이 종료되었을 때 호출
   */
  virtual void OnTurnEnd() = 0;

  /**
   * @brief 객체의 이름을 반환 (디버깅 및 UI용)
   */
  virtual std::wstring name() const = 0;

  virtual int turn_speed() const = 0;
  virtual bool IsActionFinished() const = 0;
};

/**
 * @brief 게임의 행동 순서를 관리하는 매니저 클래스
 */
class MINT_API TurnManager {
 public:
  TurnManager() = default;
  ~TurnManager() = default;

  /**
   * @brief 이번 턴에 행동 가능한 기물들을 모으기
   * @param actor 행동 가능한 기물
   * @note 기물들을 participants_에 넣음
   */
  void AddParticipant(ITurnActor* actor);

  /**
   * @brief 이번 턴에 행동 가능한 기물들을 속도 순으로 정렬하기
   * @note participants_의 기물들을 정렬해서 turn_queue_에 넣음
   */
  void StartNewRound();
  
  /**
   * @brief 다음 행동 순서로 넘어가기
   * @note turn_queue에 있는 기물들을 행동시키고 큐에서 제거함
   */
  ITurnActor* NextTurn();

  inline ITurnActor* current_actor() const { return current_actor_; }
  inline bool IsRoundFinished() const { return turn_queue_.empty(); }

 private:
  std::vector<ITurnActor*> participants_;  ///< 해당 턴에 행동 가능한 기물을 모으기 위한 변수
  std::queue<ITurnActor*> turn_queue_;  ///< 속도 순으로 정렬하여 저장하는 변수
  ITurnActor* current_actor_ = nullptr;  ///< 기물에 접근하기 위한 포인터
};

}  // namespace mint

#endif  // MINT_ENGINE_CORE_TURN_MANAGER_H_
