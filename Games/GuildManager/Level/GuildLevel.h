#ifndef GUILD_MANAGER_LEVEL_GUILD_LEVEL_H_
#define GUILD_MANAGER_LEVEL_GUILD_LEVEL_H_

#include <vector>

#include "MintEngine/Level/Level.h"

namespace mint {
namespace ui {
class TextLayout;
}
}  // namespace mint

namespace guild {
class Hero;

/**
* @class GuildLevel
 * @brief 용사를 관리하고 파티를 선발하는 메인 로비 레벨 클래스.
 * @nore 플레이어는 이 레벨에서 가용 용사 목록을 확인하고, 파티를 구성할 수 있다.
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
   * @note 향후 전용 UI 시스템(TextLayout 등)으로 기능을 이관할 예정이다.
   */
  void DrawUI(mint::Renderer& renderer);

  std::vector<Hero*> available_heroes_;  ///< 선발 가능한 용사들의 전체 목록
  std::vector<Hero*> party_;             ///< 현재 선발된 파티원 목록
  const int kMaxPartySize = 3;           ///< 파티에 편성 가능한 최대 용사 수
  int selected_index_ = 0;               ///< UI에서 현재 선택 중인 용사 인덱스

  mint::ui::TextLayout* ui_layout_ = nullptr;  ///< UI 텍스트 배치를 관리하는 레이아웃 매니저
};

}  // namespace guild

#endif  // GUILD_MANAGER_LEVEL_GUILD_LEVEL_H_
