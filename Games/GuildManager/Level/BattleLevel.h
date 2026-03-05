#ifndef GUILD_MANAGER_LEVEL_BATTLE_LEVEL_H_
#define GUILD_MANAGER_LEVEL_BATTLE_LEVEL_H_

#include <vector>

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
 * @brief 서로의 파티가 전투를 벌이는 전장 레벨
 */
class BattleLevel : public mint::Level {
  RTTI_DECLARATIONS(BattleLevel, mint::Level)

 public:
  /**
   * @brief BattleLevel 생성자
   * @param party GuildLevel에서 선발된 용사 파티 목록
   */
  explicit BattleLevel(const std::vector<Hero*>& party);
  virtual ~BattleLevel();

  /**
   * @brief 전투 초기 설정(진영 및 용사 배치)
   */
  void BeginPlay() override;

  /**
   * @brief 전투 로직 업데이트
   */
  void Tick(float delta_time) override;

  /**
   * @brief 전투 화면 및 UI 그리기
   */
  void Draw(mint::Renderer& renderer, int width, int height) override;

 private:
  /**
   * @brief 전투용 UI 그리기
   * @note 킬스코어, 데미지, 체력바 등
   */
  void DrawBattleUI(mint::Renderer& renderer);

  std::vector<Hero*> party_;     ///< 선발된 용사 파티원들
  std::vector<Enemy*> enemies_;  ///< 상대해야 할 적의 리스트

  mint::GridMap* map_ = nullptr;               ///< 전장 격자 맵
  mint::ui::TextLayout* ui_layout_ = nullptr;  ///< UI 레이아웃 매니저

  float battle_timer_ = 0.0f;  ///< 전투 턴 처리를 위한 타이머
};

}  // namespace guild
#endif  // GUILD_MANAGER_LEVEL_BATTLE_LEVEL_H_