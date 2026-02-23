#ifndef MINT_ENGINE_CORE_INPUT_H_
#define MINT_ENGINE_CORE_INPUT_H_

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 키보드 입력을 관리하는 클래스
 */
class MINT_API Input {
  friend class Engine;

  struct KeyState {
    bool is_key_down = false;
    bool was_key_down = false;
  };

 public:
  // 키가 이번 프레임에 처음 눌렸는지 확인
  bool GetKeyDown(int key_code);

  // 키가 이번 프레임에 떼어졌는지 확인
  bool GetKeyUp(int key_code);

  // 키가 눌려있는 상태인지 확인
  bool GetKey(int key_code);

  // 싱글톤 인스턴스 반환
  static Input& Get();

 private:
  Input();
  ~Input();

  // 입력 처리 (Engine에서 호출)
  void ProcessInput();

  // 이전 프레임의 입력 상태 저장 (Engine에서 호출)
  void SavePreviousInputStates();

  KeyState key_states_[255] = {};

  static Input* instance_;
};

}  // namespace mint

#endif  // MINT_ENGINE_CORE_INPUT_H_
