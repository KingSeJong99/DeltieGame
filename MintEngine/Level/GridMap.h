#ifndef MINT_ENGINE_LEVEL_GRID_MAP_H_
#define MINT_ENGINE_LEVEL_GRID_MAP_H_

#include <vector>

#include "MintEngine/Core/Common.h"
#include "MintEngine/Math/IntVector2.h"
#include "MintEngine/Math/Vector2.h"

namespace mint {

class Actor;

/**
 * @brief 격자 맵의 한 칸 정보를 담는 구조체
 */
struct Tile {
  bool is_walkable = true;  ///< 이동 가능 여부
  Actor* occupant = nullptr; ///< 현재 위치한 액터
};

/**
 * @brief 타일 기반의 격자 맵을 관리하는 클래스
 */
class MINT_API GridMap {
 public:
  /**
   * @brief GridMap 생성자
   * @param width 맵의 가로 크기 (칸 단위)
   * @param height 맵의 세로 크기 (칸 단위)
   */
  GridMap(int width, int height);

  virtual ~GridMap() = default;

  /**
   * @brief 좌표가 맵 범위 내에 있는지 확인
   */
  bool IsWithinBounds(int x, int y) const;

  /**
   * @brief 특정 좌표의 타일 정보 반환
   */
  Tile& tile(int x, int y);

  /**
   * @brief 특정 좌표의 이동 가능 여부 설정
   */
  void set_walkable(int x, int y, bool walkable);

  /**
   * @brief 격자 좌표를 월드 좌표(Renderer용)로 변환
   */
  Vector2 GridToWorld(int x, int y) const;

  /**
   * @brief 월드 좌표를 격자 좌표로 변환
   * @param world_pos 월드좌표
   * @return 
   */
  IntVector2 WorldToGrid(const Vector2& world_pos) const;

  // Getter
  inline int width() const { return width_; }
  inline int height() const { return height_; }

 private:
  int width_;                ///< 맵 가로 너비
  int height_;               ///< 맵 세로 높이
  std::vector<Tile> tiles_;  ///< 타일 데이터 배열
};

}  // namespace mint

#endif  // MINT_ENGINE_LEVEL_GRID_MAP_H_
