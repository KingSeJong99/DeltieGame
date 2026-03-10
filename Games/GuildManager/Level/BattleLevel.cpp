#include "BattleLevel.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "Actor/Enemy.h"
#include "Actor/Hero.h"
#include "GuildLevel.h"
#include "MintEngine/Core/Input.h"
#include "MintEngine/Core/Logger.h"
#include "MintEngine/Engine/Engine.h"
#include "MintEngine/Level/GridMap.h"
#include "MintEngine/Math/Vector2.h"
#include "MintEngine/Render/Renderer.h"
#include "MintUI/TextLayout.h"
#include "Util/GachaModule.h"
#include "Util/HeroFactory.h"

namespace guild {

BattleLevel::BattleLevel(const std::vector<Hero*>& party)
    : party_(party),
      battle_timer_(0.0f),
      status_(BattleStatus::kInProgress),
      result_timer_(0.0f),
      round_count_(0) {
  MINT_LOG_INFO(L"Level이 성공적으로 전환되었습니다.");
}

BattleLevel::~BattleLevel() {
  if (ui_layout_) {
    delete ui_layout_;
    ui_layout_ = nullptr;
  }
  if (map_) {
    delete map_;
    map_ = nullptr;
  }
  if (turn_manager_) {
    delete turn_manager_;
    turn_manager_ = nullptr;
  }
}

void BattleLevel::BeginPlay() {
  MINT_LOG_INFO(L"맵과 용사들을 배치하고 있어요.");

  map_ = new mint::GridMap(150, 20);
  turn_manager_ = new mint::TurnManager();

  std::ifstream map_file("Config/BattleMap.txt");
  if (map_file.is_open()) {
    for (int y = 0; y < 20; ++y) {
      for (int x = 0; x < 150; ++x) {
        int tile_type;
        if (!(map_file >> tile_type)) break;
        map_->set_walkable(x, y, (tile_type == 0));
      }
    }
    map_file.close();
  }

  for (int i = 0; i < party_.size(); i++) {
    Hero* hero = party_[i];
    if (hero == nullptr) continue;
    hero->set_grid_map(map_);
    hero->set_battle_level(this);
    turn_manager_->AddParticipant(hero);
    mint::Vector2 world_pos = map_->GridToWorld(5, 7 + (i * 2));
    world_pos.x += 6.0f; world_pos.y += 6.0f;
    hero->set_position(world_pos);
    AddNewActor(hero);
  }

  GachaModule gacha;
  for (int i = 0; i < 3; i++) {
    std::wstring grade = gacha.Draw();
    Enemy* enemy = HeroFactory::CreateRandomEnemyByGrade(grade);
    if (!enemy) enemy = HeroFactory::CreateRandomEnemyByGrade(L"F");
    if (enemy) {
      enemies_.push_back(enemy);
      enemy->set_grid_map(map_);
      enemy->set_battle_level(this);
      turn_manager_->AddParticipant(enemy);
      mint::Vector2 world_pos = map_->GridToWorld(145, 7 + (i * 2));
      world_pos.x += 6.0f; world_pos.y += 6.0f;
      enemy->set_position(world_pos);
      AddNewActor(enemy);
    }
  }

  turn_manager_->StartNewRound();
  round_count_++;
  AddCombatLog(L"--- ROUND " + std::to_wstring(round_count_) + L" ---");
  MINT_LOG_INFO(L"세팅을 완료하였습니다.");
}

std::vector<CombatUnit*> BattleLevel::GetAllUnits() {
  std::vector<CombatUnit*> units;
  for (auto* h : party_) if (h && !h->is_dead()) units.push_back(h);
  for (auto* e : enemies_) if (e && !e->is_dead()) units.push_back(e);
  return units;
}

bool BattleLevel::IsTileOccupied(int x, int y, CombatUnit* exclude) {
  for (auto* u : GetAllUnits()) {
    if (u == exclude) continue;
    mint::IntVector2 pos = map_->WorldToGrid(u->position());
    if (pos.x == x && pos.y == y) return true;
  }
  return false;
}

void BattleLevel::Tick(float delta_time) {
  if (status_ != BattleStatus::kInProgress) {
    result_timer_ += delta_time;
    if (result_timer_ >= 3.0f && mint::Input::Get().GetKeyDown(VK_SPACE)) {
      mint::Engine::Get().SetNewLevel(new GuildLevel());
      return;
    }
    Level::Tick(delta_time);
    return;
  }

  bool any_hero_alive = false;
  for (auto* h : party_) if (!h->is_dead()) any_hero_alive = true;
  bool any_enemy_alive = false;
  for (auto* e : enemies_) if (!e->is_dead()) any_enemy_alive = true;

  if (!any_enemy_alive) { status_ = BattleStatus::kVictory; return; }
  if (!any_hero_alive) { status_ = BattleStatus::kDefeat; return; }

  for (auto* h : party_) {
    if (h->is_dead()) continue;
    if (!h->target() || dynamic_cast<CombatUnit*>(h->target())->is_dead()) {
      for (auto* e : enemies_) if (!e->is_dead()) { h->set_target(e); break; }
    }
  }
  for (auto* e : enemies_) {
    if (e->is_dead()) continue;
    if (!e->target() || dynamic_cast<CombatUnit*>(e->target())->is_dead()) {
      for (auto* h : party_) if (!h->is_dead()) { e->set_target(h); break; }
    }
  }

  mint::ITurnActor* current = turn_manager_->current_actor();
  if (current == nullptr || current->IsActionFinished()) {
    if (current) current->OnTurnEnd();
    current = turn_manager_->NextTurn();
    if (current == nullptr) {
      turn_manager_->StartNewRound();
      round_count_++;
      AddCombatLog(L"--- ROUND " + std::to_wstring(round_count_) + L" ---");
      current = turn_manager_->NextTurn();
    }
    if (current) {
      current->OnTurnBegin();
      std::wstring actor_name = dynamic_cast<mint::Actor*>(current)->name();
      AddCombatLog(L"[" + actor_name + L"]의 턴 시작!");
    }
  }

  if (mint::Input::Get().GetKeyDown(VK_TAB)) {
    view_mode_ = (view_mode_ == mint::ViewMode::kGlobal) ? mint::ViewMode::kFocus : mint::ViewMode::kGlobal;
  }
  if (view_mode_ == mint::ViewMode::kFocus && current) {
    focus_target_ = dynamic_cast<mint::Actor*>(current);
  }

  Level::Tick(delta_time);
}

void BattleLevel::Draw(mint::Renderer& renderer, int width, int height) {
  if (!ui_layout_) ui_layout_ = new mint::ui::TextLayout(renderer);
  mint::IntVector2 current_grid_pos = {-1, -1};
  mint::ITurnActor* current_turn_actor = turn_manager_->current_actor();
  if (current_turn_actor) {
    mint::Actor* actor = dynamic_cast<mint::Actor*>(current_turn_actor);
    if (actor) current_grid_pos = map_->WorldToGrid(actor->position());
  }

  int cam_x = 0, cam_y = 0;
  if (view_mode_ == mint::ViewMode::kFocus && focus_target_) {
    mint::IntVector2 grid_pos = map_->WorldToGrid(focus_target_->position());
    cam_x = std::clamp(grid_pos.x - 10, 0, map_->width() - 20);
    cam_y = std::clamp(grid_pos.y - 5, 0, map_->height() - 10);
  }

  std::vector<mint::Vector2> original_positions;
  original_positions.reserve(actors_.size());
  for (auto* actor : actors_) {
    original_positions.push_back(actor->position());
    mint::IntVector2 gp = map_->WorldToGrid(actor->position());
    if (view_mode_ == mint::ViewMode::kGlobal) actor->set_position({GetGlobalX(gp.x), GetGlobalY(gp.y)});
    else actor->set_position({GetFocusX(gp.x, cam_x) + 2.0f, GetFocusY(gp.y, cam_y) + 1.0f});
  }

  DrawBattleUI(renderer, cam_x, cam_y, current_grid_pos);
  Level::Draw(renderer, width, height);
  for (size_t i = 0; i < actors_.size(); ++i) actors_[i]->set_position(original_positions[i]);
}

void BattleLevel::AddCombatLog(const std::wstring& log) {
  combat_logs_.push_back(log);
  if (combat_logs_.size() > 8) combat_logs_.erase(combat_logs_.begin());
}

void BattleLevel::DrawCombatLogs(mint::Renderer& renderer, int start_x, int start_y, int width) {
  int box_h = (int)party_.size() * 3 + 2;
  ui_layout_->DrawBox(start_x, start_y, width, box_h, L" BATTLE LOG ", mint::Color::kWhite);
  int start_rendering_y = start_y + box_h - 2;
  for (int i = 0; i < (int)combat_logs_.size(); ++i) {
    int log_idx = (int)combat_logs_.size() - 1 - i;
    int line_y = start_rendering_y - i;
    if (line_y >= start_y + 2) {
      renderer.Submit(combat_logs_[log_idx], {(float)start_x + 2, (float)line_y}, mint::Color::kWhite, mint::Color::kBlack, 200);
    }
  }
}

void BattleLevel::DrawBattleUI(mint::Renderer& renderer, int cam_x, int cam_y, const mint::IntVector2& current_grid_pos) {
  const int kMapBoxX = 5, kMapBoxY = 5, kStatusY = 30;
  int map_box_w = (view_mode_ == mint::ViewMode::kGlobal) ? (map_->width() + 2) : (20 * 6 + 2);
  DrawStatusPanels(renderer, kMapBoxX, kStatusY, kMapBoxX + map_box_w);
  if (view_mode_ == mint::ViewMode::kGlobal) DrawGlobalMap(renderer, kMapBoxX, kMapBoxY, map_box_w, current_grid_pos);
  else DrawFocusMap(renderer, kMapBoxX, kMapBoxY, cam_x, cam_y, current_grid_pos);
  if (status_ != BattleStatus::kInProgress) DrawBattleResult(renderer, kMapBoxX, kMapBoxY, map_box_w);
}

void BattleLevel::DrawStatusPanels(mint::Renderer& renderer, int start_x, int start_y, int right_edge) {
  ui_layout_->DrawBox(start_x, start_y, 45, (int)party_.size() * 3 + 2, L" PARTY STATUS ", mint::Color::kCyan);
  for (size_t i = 0; i < party_.size(); ++i) RenderUnitInfo(renderer, party_[i], (float)start_x + 1, (float)start_y + 2 + (i * 3));
  int enemy_x = (std::max)(right_edge - 45, start_x + 47);
  ui_layout_->DrawBox(enemy_x, start_y, 45, (int)enemies_.size() * 3 + 2, L" ENEMY STATUS ", mint::Color::kRed);
  for (size_t i = 0; i < enemies_.size(); ++i) RenderUnitInfo(renderer, enemies_[i], (float)enemy_x + 1, (float)start_y + 2 + (i * 3));
  int log_x = start_x + 46;
  int log_w = enemy_x - log_x - 1;
  if (log_w > 10) DrawCombatLogs(renderer, log_x, start_y, log_w);
}

void BattleLevel::DrawGlobalMap(mint::Renderer& renderer, int x, int y, int width, const mint::IntVector2& current_grid_pos) {
  ui_layout_->DrawBox(x, y, width, map_->height() + 2, L"GLOBAL ARENA", mint::Color::kRed, mint::Color::kBlack, mint::Color::kWhite, -500);
  CombatUnit* current = dynamic_cast<CombatUnit*>(turn_manager_->current_actor());
  int mp = (current && !current->is_dead()) ? current->arena_info().current_move_points : 0;
  mint::Color range_bg = (current && dynamic_cast<Enemy*>(current)) ? mint::Color::kYellow : mint::Color::kCyan;

  for (int my = 0; my < map_->height(); ++my) {
    for (int mx = 0; mx < map_->width(); ++mx) {
      bool walkable = map_->tile(mx, my).is_walkable;
      bool in_range = false;
      if (current_grid_pos.x != -1 && mp > 0 && walkable && current && current->arena_info().state == CharacterState::kDeciding) {
        int dist = std::abs(mx - current_grid_pos.x) + std::abs(my - current_grid_pos.y);
        if (dist <= mp) in_range = true;
      }
      mint::Color bg = in_range ? range_bg : mint::Color::kBrightWhite;
      if (!walkable) renderer.Submit(L"▓", {(float)x + 1 + mx, (float)y + 1 + my}, mint::Color::kGray, mint::Color::kBrightCyan, 20);
      else renderer.Submit(L" ", {(float)x + 1 + mx, (float)y + 1 + my}, mint::Color::kBrightWhite, bg, 10);
    }
  }

  if (current && current->arena_info().state == CharacterState::kDeciding) {
    mint::Color path_color = dynamic_cast<Enemy*>(current) ? mint::Color::kBrightMagenta : mint::Color::kBrightGreen;
    for (const auto& step : current->current_path()) {
      bool in_range = (std::abs(step.x - current_grid_pos.x) + std::abs(step.y - current_grid_pos.y) <= mp);
      renderer.Submit(L"o", {(float)x + 1 + step.x, (float)y + 1 + step.y}, path_color, in_range ? range_bg : mint::Color::kBrightWhite, 30);
    }
  }
}

void BattleLevel::DrawFocusMap(mint::Renderer& renderer, int x, int y, int cam_x, int cam_y, const mint::IntVector2& current_grid_pos) {
  const int kCellW = 6, kCellH = 3, kViewW = 20, kViewH = 10;
  CombatUnit* current = dynamic_cast<CombatUnit*>(turn_manager_->current_actor());
  int mp = (current && !current->is_dead()) ? current->arena_info().current_move_points : 0;
  ui_layout_->DrawBox(x, y, kViewW * kCellW + 2, kViewH * kCellH + 2, L"FOCUS VIEW", mint::Color::kCyan, mint::Color::kBlack, mint::Color::kWhite, -500);

  for (int vy = 0; vy < kViewH; ++vy) {
    int wy = cam_y + vy;
    for (int vx = 0; vx < kViewW; ++vx) {
      int wx = cam_x + vx;
      if (!map_->IsWithinBounds(wx, wy)) continue;
      float rx = (float)x + 1 + (vx * kCellW), ry = (float)y + 1 + (vy * kCellH);
      bool is_walkable = map_->tile(wx, wy).is_walkable;
      bool in_range = false;
      if (current_grid_pos.x != -1 && mp > 0 && is_walkable && current && current->arena_info().state == CharacterState::kDeciding) {
        int dist = std::abs(wx - current_grid_pos.x) + std::abs(wy - current_grid_pos.y);
        if (dist <= mp) in_range = true;
      }
      mint::Color range_bg = dynamic_cast<Enemy*>(current) ? mint::Color::kYellow : mint::Color::kCyan;
      mint::Color bg = in_range ? range_bg : mint::Color::kBrightYellow;
      for (int i = 0; i < kCellH; ++i) {
        if (!is_walkable) renderer.Submit(L"██████", {rx, ry + i}, mint::Color::kGray, mint::Color::kBlack, 20);
        else renderer.Submit(L"      ", {rx, ry + i}, mint::Color::kWhite, bg, 10);
      }
      if (current && is_walkable && current->arena_info().state == CharacterState::kDeciding) {
        mint::Color path_color = dynamic_cast<Enemy*>(current) ? mint::Color::kBrightMagenta : mint::Color::kBrightGreen;
        const auto& path = current->current_path();
        if (std::find(path.begin(), path.end(), mint::IntVector2(wx, wy)) != path.end())
          renderer.Submit(L"  o  ", {rx, ry + 1}, path_color, bg, 30);
      }
      if (is_walkable) {
        renderer.Submit(L" ", {rx + kCellW - 1, ry}, mint::Color::kWhite, mint::Color::kDarkGray, 15);
        renderer.Submit(L" ", {rx + kCellW - 1, ry + 1}, mint::Color::kWhite, mint::Color::kDarkGray, 15);
        renderer.Submit(L" ", {rx + kCellW - 1, ry + 2}, mint::Color::kWhite, mint::Color::kDarkGray, 15);
        renderer.Submit(L"      ", {rx, ry + kCellH - 1}, mint::Color::kWhite, mint::Color::kDarkGray, 15);
      }
    }
  }
}

void BattleLevel::DrawBattleResult(mint::Renderer& renderer, int map_x, int map_y, int map_w) {
  int bx = map_x + (map_w - 34) / 2, by = map_y + (map_->height() + 2 - 7) / 2;
  bool victory = (status_ == BattleStatus::kVictory);
  ui_layout_->DrawBox(bx, by, 34, 7, L" BATTLE RESULT ", victory ? mint::Color::kYellow : mint::Color::kRed, mint::Color::kBlack, victory ? mint::Color::kYellow : mint::Color::kRed, 200);
  ui_layout_->DrawTextAligned(bx, by + 3, 34, victory ? L" VICTORIOUS! " : L" DEFEATED... ", mint::ui::Alignment::Center, victory ? mint::Color::kYellow : mint::Color::kRed, mint::Color::kBlack, 201);
  ui_layout_->DrawTextAligned(bx, by + 5, 34, L"Press [Space] to Return...", mint::ui::Alignment::Center, mint::Color::kGray, mint::Color::kBlack, 201);
}

void BattleLevel::RenderUnitInfo(mint::Renderer& renderer, Hero* unit, float x, float y) {
  if (!unit) return;
  bool curr = (turn_manager_->current_actor() == unit);
  renderer.Submit((curr ? L">> " : L"   ") + unit->name(), {x, y}, unit->is_dead() ? mint::Color::kGray : mint::Color::kWhite, mint::Color::kBlack, 200);
  renderer.Submit(L"   [" + JobToWString(unit->job()) + L"]", {x, y + 1}, mint::Color::kGray, mint::Color::kBlack, 200);
  if (!unit->is_dead()) renderer.Submit(L"[" + unit->image() + L"] MP: " + std::to_wstring(unit->arena_info().current_move_points) + L"/" + std::to_wstring(unit->arena_info().max_move_points), {x + 25, y + 1}, mint::Color::kBrightCyan, mint::Color::kBlack, 200);
  float ratio = (unit->max_hp() > 0) ? (float)unit->hp() / unit->max_hp() : 0;
  mint::Color bar = unit->is_dead() ? mint::Color::kDarkGray : (ratio <= 0.2 ? mint::Color::kRed : (ratio <= 0.5 ? mint::Color::kYellow : mint::Color::kGreen));
  ui_layout_->DrawProgressBar((int)x + 17, (int)y, 10, 1, ratio, L'■', bar, mint::Color::kDarkGray, 200);
  renderer.Submit(std::to_wstring(unit->hp()) + L"/" + std::to_wstring(unit->max_hp()), {x + 29, y}, bar, mint::Color::kBlack, 200);
}

void BattleLevel::RenderUnitInfo(mint::Renderer& renderer, Enemy* unit, float x, float y) {
  if (!unit) return;
  bool curr = (turn_manager_->current_actor() == unit);
  renderer.Submit((curr ? L">> " : L"   ") + unit->name(), {x, y}, unit->is_dead() ? mint::Color::kGray : mint::Color::kBrightRed, mint::Color::kBlack, 200);
  renderer.Submit(L"   [" + JobToWString(unit->job()) + L"]", {x, y + 1}, mint::Color::kGray, mint::Color::kBlack, 200);
  if (!unit->is_dead()) renderer.Submit(L"[" + unit->image() + L"] MP: " + std::to_wstring(unit->arena_info().current_move_points) + L"/" + std::to_wstring(unit->arena_info().max_move_points), {x + 25, y + 1}, mint::Color::kYellow, mint::Color::kBlack, 200);
  float ratio = (unit->max_hp() > 0) ? (float)unit->hp() / unit->max_hp() : 0;
  ui_layout_->DrawProgressBar((int)x + 17, (int)y, 10, 1, ratio, L'■', unit->is_dead() ? mint::Color::kDarkGray : mint::Color::kYellow, mint::Color::kDarkGray, 200);
  renderer.Submit(std::to_wstring(unit->hp()) + L"/" + std::to_wstring(unit->max_hp()), {x + 29, y}, unit->is_dead() ? mint::Color::kDarkGray : mint::Color::kYellow, mint::Color::kBlack, 200);
}

}  // namespace guild
