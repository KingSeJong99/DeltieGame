#include "CombatUnit.h"

#include <algorithm>

#include "Actor/Enemy.h"
#include "Actor/Hero.h"
#include "Level/BattleLevel.h"
#include "MintEngine/Core/Logger.h"
#include "MintEngine/Level/GridMap.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Render/Renderer.h"
#include "MintEngine/Util/Pathfinder.h"

namespace guild {

CombatUnit::CombatUnit(const std::wstring& name, CharacterJob job, int hp,
                       int atk, int speed, char grade,
                       const std::wstring& image)
    : mint::Actor(name, image),
      job_(job),
      hp_(hp),
      max_hp_(hp),
      atk_(atk),
      speed_(speed),
      grade_(grade),
      default_image_(image) {
  arena_info_ = {0, 0, 0, false, false, false, 0, 0, CharacterState::kWaiting};
}

void CombatUnit::Draw(mint::Renderer& renderer, int width, int height) {
  mint::Color bg_color = mint::Color::kBlack;

  if (map_ && battle_level_) {
    mint::IntVector2 grid_pos = map_->WorldToGrid(position());
    if (map_->IsWithinBounds(grid_pos.x, grid_pos.y)) {
      bool walkable = map_->tile(grid_pos.x, grid_pos.y).is_walkable;
      bg_color = walkable ? mint::Color::kBrightWhite : mint::Color::kDarkGray;

      // 현재 턴인 유닛의 이동 범위 내에 있는지 체크하여 배경색 매핑
      CombatUnit* current = dynamic_cast<CombatUnit*>(
          battle_level_->turn_manager()->current_actor());
      if (current && !current->is_dead() &&
          current->arena_info().state == CharacterState::kDeciding) {
        int mp = current->arena_info().current_move_points;
        mint::IntVector2 cur_grid = map_->WorldToGrid(current->position());
        int dist = std::abs(grid_pos.x - cur_grid.x) +
                   std::abs(grid_pos.y - cur_grid.y);

        if (dist <= mp && walkable) {
          // 현재 행동 중인 유닛 진영에 따른 배경색 매핑
          bg_color = dynamic_cast<Enemy*>(current) ? mint::Color::kYellow
                                                   : mint::Color::kCyan;
        }
      }
    }
  }

  int final_order = 100;

  if (is_dead()) {
    renderer.Submit(image_, position(), mint::Color::kDarkGray, bg_color,
                    final_order);
  } else {
    // 진영에 따른 캐릭터 색상 결정 (아군은 Cyan, 적군은 Red)
    mint::Color unit_color =
        dynamic_cast<Hero*>(this) ? mint::Color::kBrightCyan : mint::Color::kBrightRed;

    renderer.Submit(image_, position(), unit_color, bg_color, final_order);
  }
}

void CombatUnit::Tick(float delta_time) {
  if (is_dead() || !arena_info_.is_my_turn || arena_info_.is_action_finished)
    return;

  if (!target_) {
    arena_info_.is_action_finished = true;
    return;
  }

  mint::IntVector2 my_grid = map_->WorldToGrid(position());
  mint::IntVector2 target_grid = map_->WorldToGrid(target_->position());
  int dist =
      std::abs(my_grid.x - target_grid.x) + std::abs(my_grid.y - target_grid.y);

  switch (arena_info_.state) {
    case CharacterState::kDeciding:
      if (arena_info_.current_move_points == arena_info_.max_move_points) {
        action_timer_ += delta_time;
        if (action_timer_ < 1.0f) return;
      }
      if (dist <= 1) {
        arena_info_.state = CharacterState::kAttacking;
      } else {
        if (arena_info_.current_move_points > 0 && !current_path_.empty()) {
          arena_info_.state = CharacterState::kMoving;
        } else {
          arena_info_.is_action_finished = true;
        }
      }
      action_timer_ = 0.0f;
      break;

    case CharacterState::kMoving:
      action_timer_ += delta_time;
      if (action_timer_ >= 0.2f) {
        if (!current_path_.empty() && arena_info_.current_move_points > 0) {
          mint::IntVector2 next_grid = current_path_.front();
          bool occupied = battle_level_ && battle_level_->IsTileOccupied(next_grid.x, next_grid.y, this);

          if (occupied && (arena_info_.current_move_points <= 1 ||
                           current_path_.size() <= 1)) {
            MINT_LOG_DEBUG_TAG(name(), L"이동 중지 : 다음 칸 점유됨");
            image_ = default_image_;
            arena_info_.current_move_points = 0;
            arena_info_.state = CharacterState::kDeciding;
            return;
          }

          MoveTowardsNextNode();
          arena_info_.current_move_points--;
          action_timer_ = 0.0f;

          mint::IntVector2 now_grid = map_->WorldToGrid(position());
          mint::IntVector2 tgt_grid = map_->WorldToGrid(target_->position());
          int now_dist = std::abs(now_grid.x - tgt_grid.x) +
                         std::abs(now_grid.y - tgt_grid.y);

          if (now_dist <= 1 || current_path_.empty() ||
              arena_info_.current_move_points <= 0) {
            image_ = default_image_;
            arena_info_.state = CharacterState::kDeciding;
          }
        } else {
          image_ = default_image_;
          arena_info_.state = CharacterState::kDeciding;
        }
      }
      break;

    case CharacterState::kAttacking:
      action_timer_ += delta_time;
      if (action_timer_ >= 0.5f) {
        Attack(target_);
        arena_info_.is_action_finished = true;
      }
      break;

    default:
      break;
  }
}

void CombatUnit::MoveTowardsNextNode() {
  if (current_path_.empty()) return;

  mint::IntVector2 current_grid = map_->WorldToGrid(position());
  mint::IntVector2 next_grid = current_path_.front();
  current_path_.erase(current_path_.begin());

  if (next_grid.x > current_grid.x) image_ = L"▶";
  else if (next_grid.x < current_grid.x) image_ = L"◀";
  else if (next_grid.y > current_grid.y) image_ = L"▼";
  else if (next_grid.y < current_grid.y) image_ = L"▲";

  mint::Vector2 next_world_pos = map_->GridToWorld(next_grid.x, next_grid.y);
  next_world_pos.x += 6.0f;
  next_world_pos.y += 6.0f;
  set_position(next_world_pos);
}

void CombatUnit::OnTurnBegin() {
  if (is_dead()) {
    arena_info_.is_action_finished = true;
    return;
  }

  arena_info_.is_my_turn = true;
  arena_info_.is_action_finished = false;
  arena_info_.state = CharacterState::kDeciding;

  int mp = GetCalculateMovePoints();
  arena_info_.current_move_points = mp;
  arena_info_.max_move_points = mp;
  action_timer_ = 0.0f;

  if (map_ && target_ && battle_level_) {
    std::vector<mint::IntVector2> modified_tiles;
    for (auto* unit : battle_level_->GetAllUnits()) {
      if (unit == this || unit->is_dead()) continue;
      bool is_enemy = (dynamic_cast<Hero*>(this) && dynamic_cast<Enemy*>(unit)) ||
                      (dynamic_cast<Enemy*>(this) && dynamic_cast<Hero*>(unit));
      if (is_enemy) {
        mint::IntVector2 pos = map_->WorldToGrid(unit->position());
        if (map_->tile(pos.x, pos.y).is_walkable) {
          map_->set_walkable(pos.x, pos.y, false);
          modified_tiles.push_back(pos);
        }
      }
    }

    mint::IntVector2 start = map_->WorldToGrid(position());
    mint::IntVector2 end = map_->WorldToGrid(target_->position());
    current_path_ = mint::Pathfinder::FindPath(map_, start, end);

    for (const auto& pos : modified_tiles) {
      map_->set_walkable(pos.x, pos.y, true);
    }

    if (!current_path_.empty() && current_path_.front() == start)
      current_path_.erase(current_path_.begin());
    if (!current_path_.empty() && current_path_.back() == end)
      current_path_.pop_back();
  }
}

void CombatUnit::OnTurnEnd() {
  arena_info_.is_my_turn = false;
  arena_info_.state = CharacterState::kWaiting;
  current_path_.clear();
}

bool CombatUnit::IsActionFinished() const {
  return is_dead() || arena_info_.is_action_finished;
}

void CombatUnit::TakeDamage(int damage) {
  if (is_dead()) return;
  hp_ -= damage;
  if (hp_ <= 0) {
    hp_ = 0;
    arena_info_.is_dead = true;
    arena_info_.death_count++;
  }
}

void CombatUnit::Attack(mint::Actor* target) {
  if (is_dead() || !target) return;
  MINT_LOG_DEBUG_TAG(name(), L"부모 클래스의 Attack 호출됨!");
}

int CombatUnit::GetCalculateMovePoints() const {
  int base_move = 0;
  float speed_bonus = 0.0f;
  switch (job_) {
    case CharacterJob::kHero:
    case CharacterJob::kPaladin:
    case CharacterJob::kDarkKnight: base_move = 3; speed_bonus = static_cast<float>(speed_) / 20.0f; break;
    case CharacterJob::kBowMaster:
    case CharacterJob::kMarksman:
    case CharacterJob::kNightLord:
    case CharacterJob::kShadower: base_move = 5; speed_bonus = static_cast<float>(speed_) / 15.0f; break;
    case CharacterJob::kArchMageFire:
    case CharacterJob::kArchMageIce:
    case CharacterJob::kBishop: base_move = 4; speed_bonus = static_cast<float>(speed_) / 18.0f; break;
    case CharacterJob::kAdmin: base_move = 10; speed_bonus = static_cast<float>(speed_) / 50.0f; break;
    default: base_move = 2; speed_bonus = static_cast<float>(speed_) / 25.0f; break;
  }
  return base_move + static_cast<int>(speed_bonus);
}

}  // namespace guild
