#include "MintEngine/Engine/Engine.h"

#include <Windows.h>
#include <cstdio>
#include <iostream>

#include "MintEngine/Core/Input.h"
#include "MintEngine/Core/Util.h"
#include "MintEngine/Level/Level.h"
#include "MintEngine/Render/Renderer.h"

namespace mint {

Engine* Engine::instance_ = nullptr;

Engine::Engine() {
  instance_ = this;

  // 입력 관리자 생성 (Input 클래스 리팩토링 필요할 수 있음)
  input_ = new Input();

  // 설정 로드
  LoadSetting();

  // 렌더러 생성
  renderer_ = new Renderer(Vector2(static_cast<float>(setting_.width), 
                                   static_cast<float>(setting_.height)));

  // 화면 크기 저장
  screen_size_ = Vector2(static_cast<float>(setting_.width), 
                         static_cast<float>(setting_.height));

  // 커서 끄기
  util::TurnOffCursor();
}

Engine::~Engine() {
  if (main_level_) {
    delete main_level_;
    main_level_ = nullptr;
  }

  if (input_) {
    delete input_;
    input_ = nullptr;
  }

  SafeDelete(renderer_);
}

void Engine::Run() {
  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);

  int64_t current_time = 0;
  int64_t previous_time = 0;

  LARGE_INTEGER time;
  QueryPerformanceCounter(&time);

  current_time = time.QuadPart;
  previous_time = current_time;

  setting_.framerate = (setting_.framerate == 0.0f) ? 60.0f : setting_.framerate;
  float one_frame_time = 1.0f / setting_.framerate;

  BeginPlay();

  while (!is_quit_) {
    QueryPerformanceCounter(&time);
    current_time = time.QuadPart;

    float delta_time = static_cast<float>(current_time - previous_time);
    delta_time /= static_cast<float>(frequency.QuadPart);

    if (delta_time > 0.1f) delta_time = 0.1f;

    if (delta_time >= one_frame_time) {
      // Input::ProcessInput() 등도 네임스페이스 확인 필요
      input_->ProcessInput();

      Tick(delta_time);
      Draw();

      previous_time = current_time;

      input_->SavePreviousInputStates();

      if (main_level_) {
        main_level_->ProcessAddAndDestroyActors();
      }
    }
  }

  Shutdown();
}

void Engine::QuitEngine() { 
  is_quit_ = true; 
}

void Engine::SetNewLevel(Level* new_level) {
  if (main_level_) {
    delete main_level_;
    main_level_ = nullptr;
  }
  main_level_ = new_level;
}

Engine& Engine::Get() {
  if (!instance_) {
    __debugbreak();
  }
  return *instance_;
}

void Engine::Clear(CHAR_INFO* buffer, int width, int height) {
  int total_size = width * height;
  for (int i = 0; i < total_size; ++i) {
    buffer[i].Char.UnicodeChar = L' ';
    buffer[i].Attributes = static_cast<WORD>(Color::kWhite);
  }
}

void Engine::Shutdown() {
  util::TurnOnCursor();
}

void Engine::LoadSetting() {
  FILE* file = nullptr;
  fopen_s(&file, "Config/Setting.txt", "rt");

  if (!file) {
    // 기본 설정값 사용
    setting_.framerate = 60.0f;
    setting_.width = 80;
    setting_.height = 25;
    return;
  }

  char buffer[2048] = {};
  fread(buffer, sizeof(char), 2048, file);

  char* context = nullptr;
  char* token = strtok_s(buffer, "\n", &context);

  while (token) {
    char header[32] = {};
    sscanf_s(token, "%s", header, static_cast<unsigned int>(sizeof(header)));

    if (strcmp(header, "framerate") == 0) {
      sscanf_s(token, "framerate = %f", &setting_.framerate);
    } else if (strcmp(header, "width") == 0) {
      sscanf_s(token, "width = %d", &setting_.width);
    } else if (strcmp(header, "height") == 0) {
      sscanf_s(token, "height = %d", &setting_.height);
    }

    token = strtok_s(nullptr, "\n", &context);
  }

  fclose(file);
}

void Engine::BeginPlay() {
  if (main_level_) {
    main_level_->BeginPlay();
  }
}

void Engine::Tick(float delta_time) {
  if (main_level_) {
    main_level_->Tick(delta_time);
  }
}

void Engine::Draw() {
  if (!main_level_ || !renderer_) {
    return;
  }

  renderer_->Clear();
  main_level_->Draw(*renderer_, static_cast<int>(screen_size_.x), 
                    static_cast<int>(screen_size_.y));
  
  // 렌더 큐에 쌓인 모든 명령(Submit으로 들어온 것들)을 처리한다
  renderer_->Render();

  renderer_->Present();
}


}  // namespace mint
