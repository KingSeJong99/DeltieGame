#include "Pathfinder.h"
#include "MintEngine/Level/GridMap.h"

#include <cmath>
#include <set>  // Red-Black Tree

namespace mint {

std::vector<IntVector2> Pathfinder::FindPath(GridMap* map, const IntVector2& start, const IntVector2& end) {
  if (!map || !map->IsWithinBounds(start.x, start.y) || !map->IsWithinBounds(end.x, end.y)) {
    return {};
  }

  // 우선순위 큐에는 포인터를 저장해서 복사 비용과 탐색 비용을 줄임
  auto compare = [](Node* a, Node* b) { return a->f() > b->f(); };
  std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> open_list(compare);
  
  // 좌표(y * width + x)를 키로 사용하여 생성된 노드들을 관리 (Closed List 겸용)
  std::map<int, Node*> all_nodes;

  auto get_or_create_node = [&](const IntVector2& p, int g, int h, Node* parent) {
    int key = p.y * map->width() + p.x;
    auto it = all_nodes.find(key);
    if (it == all_nodes.end()) {
      Node* new_node = new Node(p, g, h, parent);
      all_nodes[key] = new_node;
      return new_node;
    }
    return it->second;
  };

  // 시작 노드 생성 및 추가
  Node* start_node = get_or_create_node(start, 0, CalculateH(start, end), nullptr);
  open_list.push(start_node);

  Node* target_node = nullptr;

  while (!open_list.empty()) {
    Node* current = open_list.top();
    open_list.pop();

    if (current->pos == end) {
      target_node = current;
      break;
    }

    // 상하좌우 인접 4칸 탐색
    const IntVector2 dirs[] = { IntVector2::kUp, IntVector2::kDown, IntVector2::kLeft, IntVector2::kRight };
    
    for (const auto& dir : dirs) {
      IntVector2 next_pos = current->pos + dir;

      if (!map->IsWithinBounds(next_pos.x, next_pos.y)) continue;
      
      // 이동 불가능한 타일 체크 (목적지는 예외로 두어 인접 칸까지 계산 가능하게 함)
      if (!map->tile(next_pos.x, next_pos.y).is_walkable && next_pos != end) continue;

      int next_g = current->g + 1;
      int key = next_pos.y * map->width() + next_pos.x;

      auto it = all_nodes.find(key);
      if (it == all_nodes.end()) {
        Node* next_node = new Node(next_pos, next_g, CalculateH(next_pos, end), current);
        all_nodes[key] = next_node;
        open_list.push(next_node);
      } else if (next_g < it->second->g) {
        // 더 짧은 경로를 찾은 경우 업데이트
        it->second->g = next_g;
        it->second->parent = current;
        open_list.push(it->second);
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
  for (auto& pair : all_nodes) {
    delete pair.second;
  }

  return path;
}

int Pathfinder::CalculateH(const IntVector2& a, const IntVector2& b) {
  // 맨해튼 거리 계산
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

}  // namespace mint
