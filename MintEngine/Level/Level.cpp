#include "MintEngine/Level/Level.h"

#include <Windows.h>
#include <algorithm>

#include "MintEngine/Actor/Actor.h"
#include "MintEngine/Render/Renderer.h"

namespace mint {

Level::Level() {}

Level::~Level() {
  for (Actor*& actor : actors_) {
    if (actor) {
      delete actor;
      actor = nullptr;
    }
  }
  actors_.clear();
}

void Level::BeginPlay() {
  for (Actor* actor : actors_) {
    if (actor->has_began_play()) {
      continue;
    }
    actor->BeginPlay();
  }
}

void Level::Tick(float delta_time) {
  for (Actor* actor : actors_) {
    actor->Tick(delta_time);
  }
}

void Level::Draw(Renderer& renderer, int width, int height) {
  std::vector<Actor*> active_actors;
  for (Actor* actor : actors_) {
    if (actor && actor->is_active()) {
      active_actors.push_back(actor);
    }
  }

  std::sort(active_actors.begin(), active_actors.end(), [](Actor* a, Actor* b) {
    return a->sorting_order() < b->sorting_order();
  });

  for (auto* actor : active_actors) {
    actor->Draw(renderer.GetFrameBuffer(), width, height);
  }
}

void Level::AddNewActor(Actor* new_actor) {
  if (new_actor == nullptr) return;
  
  add_requested_actors_.emplace_back(new_actor);
  new_actor->set_owner(this);
}

void Level::ProcessAddAndDestroyActors() {
  // 제거 처리
  for (auto it = actors_.begin(); it != actors_.end(); ) {
    if ((*it)->destroy_requested()) {
      delete *it;
      it = actors_.erase(it);
    } else {
      ++it;
    }
  }

  // 추가 처리
  if (add_requested_actors_.empty()) {
    return;
  }

  for (Actor* const actor : add_requested_actors_) {
    actors_.emplace_back(actor);
  }
  add_requested_actors_.clear();
}

}  // namespace mint
