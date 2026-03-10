#ifndef GUILD_MANAGER_LEVEL_BATTLE_LEVEL_H_
#define GUILD_MANAGER_LEVEL_BATTLE_LEVEL_H_

#include <vector>

#include "MintEngine/Core/Camera.h"
#include "MintEngine/Core/TurnManager.h"
#include "MintEngine/Level/Level.h"
#include "MintEngine/Math/IntVector2.h"

namespace mint {
class GridMap;
namespace ui {
class TextLayout;
}
}  // namespace mint

namespace guild {

class CombatUnit;
class Hero;
class Enemy;

/**
 * @enum BattleStatus
 * @brief 전투의 현재 진행 상태를 나타내는 열거형.
 */
enum class BattleStatus {
  kInProgress,  ///< 전투 진행 중
  kVictory,     ///< 아군 승리
  kDefeat       ///< 아군 패배
};

/**
 * @class BattleLevel
 * @brief 플레이어의 용사 파티와 적군 간의 턴제 전투를 관리하는 전장 레벨
 * 클래스.
 * @details 맵 로드, 유닛 배치, 턴 순서 제어 및 전장 렌더링(Global/Focus 모드)을
 * 담당한다.
 */
class BattleLevel : public mint::Level {
  RTTI_DECLARATIONS(BattleLevel, mint::Level)

 public:
  /**
   * @brief BattleLevel 생성자.
   * @param party GuildLevel에서 선발된 용사 객체들의 목록.
   */
  explicit BattleLevel(const std::vector<Hero*>& party);

  /** @brief BattleLevel 소멸자. 할당된 메모리(UI, Map, TurnManager)를 정리한다.
   */
  virtual ~BattleLevel();

  /** @brief 전장을 초기화한다. 맵을 로드하고 아군/적군을 배치한다. */
  void BeginPlay() override;

  /** @brief 매 프레임 전투 로직을 업데이트한다. (턴 전환, 생존 확인 등) */
  void Tick(float delta_time) override;

  /** @brief 전장과 UI를 화면에 렌더링한다. */
  void Draw(mint::Renderer& renderer, int width, int height) override;

  // --- [좌표 변환 도우미 함수] ---

  /** @brief Global 모드용 X 좌표 변환 (그리드 -> 스크린) */
  inline float GetGlobalX(int x) { return 6.0f + x; }
  /** @brief Global 모드용 Y 좌표 변환 (그리드 -> 스크린) */
  inline float GetGlobalY(int y) { return 6.0f + y; }
  /** @brief Focus 모드용 X 좌표 변환 (카메라 오프셋 적용 및 6배 확대) */
  inline float GetFocusX(int x, int cam_x) { return 6.0f + (x - cam_x) * 6; }
  /** @brief Focus 모드용 Y 좌표 변환 (카메라 오프셋 적용 및 3배 확대) */
  inline float GetFocusY(int y, int cam_y) { return 6.0f + (y - cam_y) * 3; }

  /** @brief 전장에 존재하는 모든 살아있는 유닛 목록을 반환한다. */
  std::vector<CombatUnit*> GetAllUnits();
  /** @brief 특정 그리드 좌표에 유닛이 존재하는지 확인한다. */
  bool IsTileOccupied(int x, int y, CombatUnit* exclude = nullptr);

  /** @brief 현재 턴 매니저를 반환한다. */
  mint::TurnManager* turn_manager() const { return turn_manager_; }

  /** @brief 전투 로그를 추가한다. */
  void AddCombatLog(const std::wstring& log);

 private:
  // --- [내부 렌더링 세부 구현] ---

  /** @brief 화면 중앙에 전투 로그를 출력한다. */
  void DrawCombatLogs(mint::Renderer& renderer, int start_x, int start_y, int width);

  /**
   * @brief 통합 전투 UI를 렌더링한다.
   * @param current_grid_pos 현재 턴인 유닛의 실시간 그리드 좌표 (시각화용 중요
   * 데이터!)
   */
  void DrawBattleUI(mint::Renderer& renderer, int cam_x, int cam_y,
                    const mint::IntVector2& current_grid_pos);

  /** @brief 아군 및 적군의 상태 패널(이름, HP바, MP 등)을 그린다. */
  void DrawStatusPanels(mint::Renderer& renderer, int start_x, int start_y,
                        int right_edge);

  /** @brief 전장 전체를 한눈에 보여주는 Global 맵을 그린다. (이동 범위/경로
   * 포함) */
  void DrawGlobalMap(mint::Renderer& renderer, int x, int y, int width,
                     const mint::IntVector2& current_grid_pos);

  /** @brief 특정 유닛을 확대한 Focus 맵을 픽셀 스타일로 그린다. (이동 범위/경로
   * 포함) */
  void DrawFocusMap(mint::Renderer& renderer, int x, int y, int cam_x,
                    int cam_y, const mint::IntVector2& current_grid_pos);

  /** @brief 전투 종료 시 승리/패배 결과 박스를 맵 중앙에 띄운다. */
  void DrawBattleResult(mint::Renderer& renderer, int map_x, int map_y,
                        int map_w);

  /** @brief 아군 용사의 세부 정보를 렌더링한다. */
  void RenderUnitInfo(mint::Renderer& renderer, Hero* unit, float x, float y);

  /** @brief 적군 유닛의 세부 정보를 렌더링한다. */
  void RenderUnitInfo(mint::Renderer& renderer, Enemy* unit, float x, float y);

  // --- [데이터 멤버] ---

  std::vector<Hero*> party_;     ///< 아군 용사 파티
  std::vector<Enemy*> enemies_;  ///< 적군 리스트
  std::vector<std::wstring> combat_logs_; ///< 전투 로그 저장소

  mint::GridMap* map_ = nullptr;               ///< 전장 격자 데이터
  mint::ui::TextLayout* ui_layout_ = nullptr;  ///< 텍스트 UI 배치 매니저
  mint::TurnManager* turn_manager_ = nullptr;  ///< 턴 제어 시스템

  BattleStatus status_ = BattleStatus::kInProgress;  ///< 현재 전투 상태
  int round_count_ = 0;                              ///< 현재 라운드 수
  float result_timer_ = 0.0f;                        ///< 결과 출력 대기 타이머
  float battle_timer_ = 0.0f;                        ///< 전체 전투 시간 기록용

  mint::ViewMode view_mode_ =
      mint::ViewMode::kGlobal;           ///< 현재 뷰 모드 (Global/Focus)
  mint::Actor* focus_target_ = nullptr;  ///< Focus 모드에서 추적할 대상
};

}  // namespace guild
#endif  // GUILD_MANAGER_LEVEL_BATTLE_LEVEL_H_
