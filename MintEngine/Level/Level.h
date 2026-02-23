#ifndef MINT_ENGINE_LEVEL_LEVEL_H_
#define MINT_ENGINE_LEVEL_LEVEL_H_

#include <Windows.h>
#include <vector>

#include "MintEngine/Core/RTTI.h"

namespace mint {

class Actor;
class Renderer;

/**
 * @brief 게임 내 모든 액터를 관리하는 클래스
 */
class MINT_API Level : public RTTI {
  RTTI_DECLARATIONS(Level, RTTI)

 public:
  Level();
  virtual ~Level();

  // 레벨 시작 및 업데이트 이벤트
  virtual void BeginPlay();
  virtual void Tick(float delta_time);
  virtual void Draw(Renderer& renderer, int width, int height);

  // 새로운 액터 추가 요청
  void AddNewActor(Actor* new_actor);

  // 액터의 추가 및 삭제 요청을 실제로 처리
  void ProcessAddAndDestroyActors();

 protected:
  // 현재 레벨에 존재하는 액터 목록
  std::vector<Actor*> actors_;

  // 다음 프레임에 추가될 예정인 액터 목록
  std::vector<Actor*> add_requested_actors_;
};

}  // namespace mint

#endif  // MINT_ENGINE_LEVEL_LEVEL_H_
