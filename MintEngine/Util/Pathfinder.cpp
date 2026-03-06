#include "Pathfinder.h"
#include "MintEngine/Level/GridMap.h"

#include <cmath>
#include <set>  // Red-Black Tree

namespace mint {

std::vector<IntVector2> Pathfinder::FindPath(GridMap* map, const IntVector2& start, const IntVector2& end) {
  if (!map || !map->IsWithinBounds(start.x, start.y) || !map->IsWithinBounds(end.x, end.y)) {
    return {};
  }

  // 목적지가 이동 불가능한 곳이면 바로 종료 (상황에 따라 다를 수 있음)
  if (!map->tile(end.x, end.y).is_walkable) {
    // return {}; // 일단은 주석 처리 (적군 옆 칸까지만 가면 되니까)
  }

  // 우선순위 큐 (Open List)
  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_list;
  
  // 방문 여부 및 비용 기록 (Closed List 대용)
  std::map<int, int> visited_g; // key: y * width + x, value: g_cost

  // 메모리 관리를 위한 모든 생성된 노드 저장소 (간단하게 구현하기 위함)
  std::vector<Node*> all_nodes;

  auto create_node = [&](IntVector2 p, int g, int h, Node* parent) {
    Node* new_node = new Node(p, g, h, parent);
    all_nodes.push_back(new_node);
    return new_node;
  };

  // 시작 노드 추가
  open_list.push(*create_node(start, 0, CalculateH(start, end), nullptr));
  visited_g[start.y * map->width() + start.x] = 0;

  Node* target_node = nullptr;

  while (!open_list.empty()) {
    Node current = open_list.top();
    open_list.pop();

    // 목적지 도착!
    if (current.pos == end) {
      // 큐에서 꺼낸 데이터는 복사본이므로 실제 all_nodes에 있는 노드를 찾아야 함
      for (auto* n : all_nodes) {
        if (n->pos == current.pos && n->g == current.g) {
          target_node = n;
          break;
        }
      }
      break;
    }

    // 상하좌우 인접 4칸 탐색
    IntVector2 dirs[] = { IntVector2::kUp, IntVector2::kDown, IntVector2::kLeft, IntVector2::kRight };
    
    for (const auto& dir : dirs) {
      IntVector2 next_pos = current.pos + dir;

      if (!map->IsWithinBounds(next_pos.x, next_pos.y)) continue;
      if (!map->tile(next_pos.x, next_pos.y).is_walkable && next_pos != end) continue;

      int next_g = current.g + 1;
      int key = next_pos.y * map->width() + next_pos.x;

      if (visited_g.find(key) == visited_g.end() || next_g < visited_g[key]) {
        visited_g[key] = next_g;
        
        // 현재 노드 포인터 찾기 (부모 설정을 위해)
        Node* current_ptr = nullptr;
        for (auto* n : all_nodes) {
          if (n->pos == current.pos && n->g == current.g) {
            current_ptr = n;
            break;
          }
        }

        open_list.push(*create_node(next_pos, next_g, CalculateH(next_pos, end), current_ptr));
      }
    }
  }

  // 경로 복원
  std::vector<IntVector2> path;
  if (target_node) {
    Node* curr = target_node;
    while (curr) {
      path.push_back(curr->pos);
      curr = curr->parent;
    }
    std::reverse(path.begin(), path.end());
  }

  // 메모리 해제
  for (auto* n : all_nodes) delete n;

  return path;
}

int Pathfinder::CalculateH(const IntVector2& a, const IntVector2& b) {
  // 맨해튼 거리 계산
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

}  // namespace mint
