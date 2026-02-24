#ifndef GUILD_MANAGER_LEVEL_GUILD_LEVEL_H_
#define GUILD_MANAGER_LEVEL_GUILD_LEVEL_H_

#include <vector>

#include "MintEngine/Level/Level.h"

namespace guild {
class Hero;

/**
 * @brief 용사를 관리하고 파티를 선발하는 메인 로비 레벨
 */
class GuildLevel : public mint::Level {
  RTTI_DECLARATIONS(GuildLevel, mint::Level)

 public:
  GuildLevel();
  virtual ~GuildLevel();

  /**
   * @brief 레벨 시작 시 초기화 (용사 생성 등)
   */
  void BeginPlay() override;

  /**
   * @brief 매 프레임 업데이트 (입력 처리 등)
   * @param delta_time 프레임 간 경과 시간
   */
  void Tick(float delta_time) override;

  /**
   * @brief 화면 그리기 (UI 테두리 등)
   * @param renderer 그리기를 요청할 객체
   * @param width 그리기를 수행할 위치의 가로 폭
   * @param height 그리기를 수행할 위치의 세로 폭
   */
  void Draw(mint::Renderer& renderer, int width, int height) override;

 private:
  /**
   * @brief 화면에 고정된 UI(박스 등)을 그리는 함수
   * @param renderer 그리기를 요청할 객체
   * @note UI를 그리는 전용 유틸을 생성할 예정
   */
  void DrawUI(mint::Renderer& renderer);

  std::vector<Hero*> available_heroes_;  ///< 선발 가능한 용사 목록
  int selected_index_ = 0;				 ///< 현재 선택 중인 용사 인덱스
};

}  // namespace guild

#endif  // GUILD_MANAGER_LEVEL_GUILD_LEVEL_H_


