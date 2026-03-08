#include "MintEngine/Level/Level.h"

#include <Windows.h>

#include <algorithm>

#include "MintEngine/Actor/Actor.h"
#include "MintEngine/Core/Logger.h"
#include "MintEngine/Engine/Engine.h"
#include "MintEngine/Render/Renderer.h"

namespace mint {

Level::Level() {
  Engine* engine_ptr = &Engine::Get();
  if (!engine_ptr) {
    MINT_LOG_ERROR(L"MintEngine이 존재하지 않습니다 !! ");
    __debugbreak();
  }
}

Level::~Level() {
  for (Actor*& actor : actors_) {
    if (actor) {
      delete actor;
      actor = nullptr;
    }
  }
  actors_.clear();

  MINT_LOG_INFO(L"Level을 소멸합니다. (주소: " +
                              std::to_wstring((uintptr_t)this) + L")");
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
  if (new_actor == nullptr) {
    MINT_LOG_WARN(L"null 액터가 전달되었습니다.");
    return;
  }
  add_requested_actors_.emplace_back(new_actor);
  new_actor->set_owner(this);

  // Todo: 액터의 헬퍼 함수로 어떤 액터인지 확인 가능하게 하기
  MINT_LOG_DEBUG_TAG(new_actor->name(), L"액터가 추가 예약되었어요.");
}

void Level::ProcessAddAndDestroyActors() {
  // 제거 처리
  int destroyed_count = 0;  ///< 로그용 변수
  for (auto it = actors_.begin(); it != actors_.end();) {
    if ((*it)->destroy_requested()) {
      delete *it;
      it = actors_.erase(it);
      ++destroyed_count;
    } else {
      ++it;
    }
  }
  if (destroyed_count > 0) {
    MINT_LOG_INFO(std::to_wstring(destroyed_count) +
                  L"개의 액터가 레벨에서 제거되었어요.");
  }

  // 추가 처리
  if (add_requested_actors_.empty()) {
    return;
  }

  size_t added_count = add_requested_actors_.size();
  for (Actor* const actor : add_requested_actors_) {
    actors_.emplace_back(actor);
  }
  add_requested_actors_.clear();
  MINT_LOG_INFO(std::to_wstring(added_count) +
                L"개의 액터가 레벨에 최종 추가됐어요.");
}

void Level::RemoveActorWithoutDeleting(Actor* actor) {
  if (actor == nullptr) {
    MINT_LOG_WARN(L"null 액터가 전달되었습니다.");
    return;
  }

  auto it = std::find(actors_.begin(), actors_.end(), actor);
  if (it != actors_.end()) {
    actors_.erase(it);
    MINT_LOG_DEBUG_TAG(actor->name(),
                       L"액터가 레벨 관리 목록에서 제거되었어요.");
  }
}
}  // namespace mint
