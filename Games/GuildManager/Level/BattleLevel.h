#ifndef GUILD_MANAGER_LEVEL_BATTLE_LEVEL_H_
#define GUILD_MANAGER_LEVEL_BATTLE_LEVEL_H_

#include <vector>

#include "MintEngine/Core/TurnManager.h"
#include "MintEngine/Level/Level.h"

namespace mint {
class GridMap;
namespace ui {
class TextLayout;
}
}  // namespace mint

namespace guild {

class Hero;
class Enemy;

/**
 * @class BattleStatus 
 * @brief 전투 상태를 나타내는 열거형.
 */
enum class BattleStatus {kInProgress, kVictory, kDefeat};

/**
 * @class BattleLevel
 * @brief 플레이어의 파티와 상대 파티 간의 턴제 전투를 관리하는 전장 레벨
 * 클래스.
 * @note 턴 매니저를 통해 전투 순서를 제어하고, 그리드 맵 위에서 액터들의
 * 상호작용을 처리한다.
 */
class BattleLevel : public mint::Level {
  RTTI_DECLARATIONS(BattleLevel, mint::Level)

 public:
  /**
   * @brief BattleLevel 생성자.
   * @param party GuildLevel에서 선발된 용사 파티 목록.
   * @note 전달받은 용사 포인터의 소유권 처리에 주의해야한다.
   */
  explicit BattleLevel(const std::vector<Hero*>& party);
  virtual ~BattleLevel();

  /**
   * @brief 전장을 초기화하고 용사와 적군을 그리드 맵에 배치한다.
   */
  void BeginPlay() override;

  /**
   * @brief 전투 타이머 및 턴 상태를 매 프레임 업데이트한다.
   * @param delta_time 프레임 간 경과 시간(초).
   */
  void Tick(float delta_time) override;

  /**
   * @brief 전장 맵, 액터, 그리고 전투 UI를 렌더링한다.
   * @param render 렌더링을 수행할 렌더러 객체.
   * @param width 화면 가로 해상도.
   * @param height 화면 세로 해상도.
   */
  void Draw(mint::Renderer& renderer, int width, int height) override;

 private:
  /**
   * @brief 현재 전투 상태(체력바, 턴 정보 등)를 화면에 렌더링한다.
   * @param renderer 렌더링을 수행할 렌더러 객체.
   * @note 킬스코어, 데미지, 체력바 등.
   */
  void DrawBattleUI(mint::Renderer& renderer);

  std::vector<Hero*> party_;     ///< 선발된 용사 파티원들
  std::vector<Enemy*> enemies_;  ///< 상대해야 할 적의 리스트

  mint::GridMap* map_ = nullptr;  ///< 전장 격자 맵
  mint::ui::TextLayout* ui_layout_ =
      nullptr;  ///< 전투 UI 텍스트 배치를 관리하는 매니저
  mint::TurnManager* turn_manager_ =
      nullptr;  ///< 전투의 턴 순서 및 진행을 관리하는 시스템

  BattleStatus status_ = BattleStatus::kInProgress;  ///< 현재 전투 상태
  float result_timer_ = 0.0f;  ///< 전투 종료 후 대기 시간을 위한 타이머

  float battle_timer_ =
      0.0f;  ///< 전투 진행 시간 및 턴 전환 대기 처리를 위한 타이머
};

}  // namespace guild
#endif  // GUILD_MANAGER_LEVEL_BATTLE_LEVEL_H_