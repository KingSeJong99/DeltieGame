#ifndef GUILD_MANAGER_LEVEL_GUILD_LEVEL_H_
#define GUILD_MANAGER_LEVEL_GUILD_LEVEL_H_

#include <vector>

#include "MintEngine/Level/Level.h"
#include "Util/GachaModule.h"
#include "Util/GameDataManager.h"
#include "Util/HeroFactory.h"

namespace mint {
namespace ui {
class TextLayout;
}
}  // namespace mint

namespace guild {
class Hero;

/**
 * @enum GuildState
 * @brief GuildLevel의 화면을 전환한다.
 */
enum class GuildState {
  kManagement,  ///< 대시보드
  kGacha        ///< 가차

};

/**
 * @class GuildLevel
 * @brief 용사를 관리하고 파티를 선발하는 메인 로비 레벨 클래스.
 * @note 플레이어는 이 레벨에서 가용 용사 목록을 확인하고, 파티를 구성할 수
 * 있다.
 */
class GuildLevel : public mint::Level {
  RTTI_DECLARATIONS(GuildLevel, mint::Level)

 public:
  GuildLevel();
  virtual ~GuildLevel();

  /**
   * @brief 레벨 시작 시 호출되며, 초기 용사 목록을 생성하고 UI를 설정한다.
   */
  void BeginPlay() override;

  /**
   * @brief 매 프레임 업데이트 로직을 수행한다.
   * @param delta_time 프레임 간 경과 시간(초).
   */
  void Tick(float delta_time) override;

  /**
   * @brief 레벨의 배경과 UI 요소를 화면에 그린다.
   * @param renderer 렌더링을 수행할 렌더러 객체.
   * @param width 화면 가로 해상도.
   * @param height 화면 세로 해상도.
   */
  void Draw(mint::Renderer& renderer, int width, int height) override;

 private:
  /**
   * @brief 화면에 고정된 UI 프레임(박스 등)을 렌더링한다.
   * @param renderer 렌더링을 수행할 렌더러 객체
   */
  void DrawUI(mint::Renderer& renderer);

  /**
   * @brief 구역별 렌더링 함수
   */
  void DrawHeroList(mint::Renderer& renderer, int x, int y, int w, int h);
  void DrawHeroDetail(mint::Renderer& renderer, int x, int y, int w, int h);
  void DrawPartyStatus(mint::Renderer& renderer, int x, int y, int w, int h);

  void DrawGachaUI(mint::Renderer& renderer);

  // 데이터 멤버
  GachaModule* gacha_module_ = nullptr;  ///< 가챠 시스템 객체
  GuildState current_state_ = GuildState::kManagement;
  GameDataManager& data_ = GameDataManager::Get();

  const int kMaxPartySize = 3;  ///< 파티에 편성 가능한 최대 용사 수
  int selected_index_ = 0;      ///< UI에서 현재 선택 중인 용사 인덱스

  mint::ui::TextLayout* ui_layout_ =
      nullptr;  ///< UI 텍스트 배치를 관리하는 레이아웃 매니저

  std::vector<Hero*> last_pull_results_;  ///< 방금 뽑은 결과들 (연출용)
  float gacha_timer_ = 0.0f;              ///< 연출용 타이머
  int reveal_count_ = 0;                  ///< 현재까지 공개된 카드 수
  bool is_animating_ = false;             ///< 연출 중인지 여부
};

}  // namespace guild

#endif  // GUILD_MANAGER_LEVEL_GUILD_LEVEL_H_