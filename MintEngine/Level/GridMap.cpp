#include "MintEngine/Level/GridMap.h"

#include <stdexcept>

namespace mint {

GridMap::GridMap(int width, int height) : width_(width), height_(height) {
  if (width_ <= 0 || height_ <= 0) {
    throw std::invalid_argument("GridMap dimensions must be greater than 0.");
  }
  // width * height 크기만큼 Tile 객체를 기본값으로 채움
  tiles_.resize(width_ * height_);
}

bool GridMap::IsWithinBounds(int x, int y) const {
  return (x >= 0 && x < width_ && y >= 0 && y < height_);
}

Tile& GridMap::tile(int x, int y) {
  if (!IsWithinBounds(x, y)) {
    // 맵 밖을 참조하려고 하면 에러를 뿜게 할 수도 있지만, 
    // 게임에서는 보통 안전하게 처리해야 함. 여기선 일단 예외를 던짐.
    throw std::out_of_range("GridMap::GetTile - Coordinates out of bounds.");
  }
  return tiles_[y * width_ + x];
}

void GridMap::set_walkable(int x, int y, bool walkable) {
  if (IsWithinBounds(x, y)) {
    tiles_[y * width_ + x].is_walkable = walkable;
  }
}

Vector2 GridMap::GridToWorld(int x, int y) const {
  // 현재는 1:1 매핑. 필요하다면 여기에 타일 크기 계수를 곱하면 됨.
  // 예: 가로폭 보정을 위해 x * 2.0f
  return Vector2(static_cast<float>(x), static_cast<float>(y));
}

IntVector2 GridMap::WorldToGrid(const Vector2& world_pos) const {
  // float 좌표를 int 격자 좌표로 내림(버림) 처리
  int grid_x = static_cast<int>(world_pos.x);
  int grid_y = static_cast<int>(world_pos.y);
  return IntVector2(grid_x, grid_y);
}

}  // namespace mint
