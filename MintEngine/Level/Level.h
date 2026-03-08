#ifndef MINT_ENGINE_LEVEL_LEVEL_H_
#define MINT_ENGINE_LEVEL_LEVEL_H_

#include <Windows.h>
#include <vector>

#include "MintEngine/Core/RTTI.h"

namespace mint {

class Actor;
class Renderer;

/**
 * @class Level
 * @brief 게임의 장면(Scene)을 나타내며, 그 안에 속한 모든 액터들을 관리하는 컨테이너 클래스.
 * @note 액터의 생성, 업데이트, 렌더링 및 삭제를 총괄한다.
 * 프레임 도중 액터 목록이 변하는 것을 방지하기 위해 지연된 추가/삭제 매커니즘을 사용한다.
 */
class MINT_API Level : public RTTI {
  RTTI_DECLARATIONS(Level, RTTI)

 public:
  Level();
  virtual ~Level();

  /**
   * @brief 레벨이 시작될 때 호출되는 초기화 이벤트 함수.
   */
  virtual void BeginPlay();

  /**
   * @brief 매 프레임마다 레벨 내 모든 액터의 로직을 업데이트한다.
   * @param delta_time 이전 프레임부터의 경과 시간(초).
   */
  virtual void Tick(float delta_time);

  /**
   * @brief 레벨 내 모든 액터를 화면에 그린다.
   * @param renderer 렌더링을 담당할 렌더링 객체.
   * @param width 화면 가로 크기.
   * @param height 화면 세로 크기.
   */
  virtual void Draw(Renderer& renderer, int width, int height);

  /**
   * @brief 새로운 액터를 레벨에 추가하도록 요청한다.
   * @param new_actor 추가할 액터의 포인터.
   * @note 즉시 추가되지 않고 'add_requested_actors_'에 보관되었다가,
   * 다음 프레임의 'ProcessAddAndDestoryActors' 호출 시점에 실제 액터 목록에 반영된다.
   */
  void AddNewActor(Actor* new_actor);

  /**
   * @brief 예약된 액터의 추가 및 삭제 요청을 실제로 처리하여 액터 목록을 갱신한다.
   * @note 이 함수는 보통 프레임의 마지막이나 시작 부분에서 한 번만 호출되어야 한다.
   */
  void ProcessAddAndDestroyActors();

  /**
   * @brief 액터를 레벨의 관리 목록에서 제외한다. (메모리는 해제하지 않음)
   * @param actor 제외할 액터 포인터.
   * @note Level 전환 시, 이동할 액터 목록을 현재 레벨에서 소유권을 넘기는 것이기 때문에
   * 소유권을 이전받은 Level은 반드시 메모리 해제를 해야한다
   */
  void RemoveActorWithoutDeleting(Actor* actor);

 protected:

  std::vector<Actor*> actors_;  ///< 현재 레벨에서 활성화된 액터 목록

  std::vector<Actor*> add_requested_actors_;  ///< 다음 프레임에 추가될 예정인 액터 목록
};

}  // namespace mint

#endif  // MINT_ENGINE_LEVEL_LEVEL_H_
