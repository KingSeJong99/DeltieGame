#ifndef MINT_ENGINE_UTIL_PATHFINDER_H_
#define MINT_ENGINE_UTIL_PATHFINDER_H_

#include <vector>
#include <queue>
#include <map>

#include "MintEngine/Core/Common.h"
#include "MintEngine/Math/IntVector2.h"

namespace mint {

class GridMap;

/**
 * @brief A* 알고리즘을 위한 노드 구조체
 * @note A* 공식 f(n) = g(n) + h(n)
 */
struct Node {
  IntVector2 pos;
  int g; // 시작점부터의 비용
  int h; // 목적지까지의 예상 비용
  int f() const { return g + h; }
  Node* parent = nullptr;

  Node(IntVector2 p, int g_val, int h_val, Node* parent_node = nullptr)
      : pos(p), g(g_val), h(h_val), parent(parent_node) {}

  // 우선순위 큐를 위한 비교 연산자 (F값이 낮은 게 우선순위가 높음)
  bool operator>(const Node& other) const {
    return f() > other.f();
  }
};

/**
 * @brief 격자 맵에서 최단 경로를 찾아주는 유틸리티 클래스
 */
class MINT_API Pathfinder {
 public:
  /**
   * @brief 시작점에서 목적지까지의 최단 경로를 반환
   * @param map 탐색할 격자 맵
   * @param start 시작 격자 좌표
   * @param end 목적지 격자 좌표
   * @return 경로 상의 격자 좌표 리스트 (찾지 못하면 빈 리스트)
   */
  static std::vector<IntVector2> FindPath(GridMap* map, const IntVector2& start, const IntVector2& end);

 private:
  /**
   * @brief 두 좌표 사이의 맨해튼 거리를 계산 (Heuristic)
   * @param a 첫 번째 좌표
   * @param b 목표 좌표
   * @return h(n) 값
   * @note 이동 한 번으로 상하좌우 이동 가능한 상황이기 때문에 맨해튼 거리가 최적. 장애물이 없을 때,
   * 실제 거리와 같거나 장애물이 있다면 실제 거리보다 맨해튼 거리가 짧기 때문에 허용성을 만족함
   */
  static int CalculateH(const IntVector2& a, const IntVector2& b);
};

}  // namespace mint

#endif  // MINT_ENGINE_UTIL_PATHFINDER_H_
