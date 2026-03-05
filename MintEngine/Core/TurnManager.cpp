#include "MintEngine/Core/TurnManager.h"

#include <algorithm>

namespace mint {

void TurnManager::AddParticipant(ITurnActor* actor) {
  if (actor == nullptr) return;
  participants_.push_back(actor);
}

void TurnManager::StartNewRound() {
  // 1. 기존 큐 비우기
  while (!turn_queue_.empty()) {
    turn_queue_.pop();
  }

  // 2. 참여자 정렬 (속도 내림차순)
  std::sort(participants_.begin(), participants_.end(),
            [](ITurnActor* a, ITurnActor* b) {
              return a->turn_speed() > b->turn_speed();
            });

  // 3. 큐에 다시 채우기
  for (auto participant : participants_) {
    turn_queue_.push(participant);
  }
  
  current_actor_ = nullptr;
}

ITurnActor* TurnManager::NextTurn() {
  // 1. 현재 행동 중인 액터가 있다면 종료 처리
  if (current_actor_) {
    current_actor_->OnTurnEnd();
  }

  // 2. 다음 액터가 있는지 확인
  if (turn_queue_.empty()) {
    current_actor_ = nullptr;
    return nullptr;
  }

  // 3. 큐에서 꺼내서 현재 액터로 설정
  current_actor_ = turn_queue_.front();
  turn_queue_.pop();

  // 4. 새 액터 시작 처리
  if (current_actor_) {
    current_actor_->OnTurnBegin();
  }

  return current_actor_;
}

}  // namespace mint
