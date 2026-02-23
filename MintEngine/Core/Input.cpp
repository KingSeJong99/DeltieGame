#include "MintEngine/Core/Input.h"

#include <Windows.h>
#include <iostream>

namespace mint {

Input* Input::instance_ = nullptr;

Input::Input() {
  instance_ = this;
}

Input::~Input() {}

bool Input::GetKeyDown(int key_code) {
  if (key_code < 0 || key_code >= 255) return false;
  return key_states_[key_code].is_key_down && !key_states_[key_code].was_key_down;
}

bool Input::GetKeyUp(int key_code) {
  if (key_code < 0 || key_code >= 255) return false;
  return !key_states_[key_code].is_key_down && key_states_[key_code].was_key_down;
}

bool Input::GetKey(int key_code) {
  if (key_code < 0 || key_code >= 255) return false;
  return key_states_[key_code].is_key_down;
}

Input& Input::Get() {
  if (!instance_) {
    __debugbreak();
  }
  return *instance_;
}

void Input::ProcessInput() {
  for (int i = 0; i < 255; ++i) {
    key_states_[i].is_key_down = (GetAsyncKeyState(i) & 0x8000) != 0;
  }
}

void Input::SavePreviousInputStates() {
  for (int i = 0; i < 255; ++i) {
    key_states_[i].was_key_down = key_states_[i].is_key_down;
  }
}

}  // namespace mint
