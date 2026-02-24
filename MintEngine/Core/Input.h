#ifndef MINT_ENGINE_CORE_INPUT_H_
#define MINT_ENGINE_CORE_INPUT_H_

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 키보드 입력 상태를 관리하는 싱글톤 클래스
 */
class MINT_API Input {
  friend class Engine;

  /**
   * @brief 키의 현재 및 이전 프레임 상태를 담는 구조체
   */
  struct KeyState {
    bool is_key_down = false;   ///< 현재 프레임에서 눌려있는지 여부
    bool was_key_down = false;  ///< 이전 프레임에서 눌려있었는지 여부
  };

 public:
  /**
   * @brief 키가 이번 프레임에 처음 눌렸는지 확인 (Down Event)
   * @param key_code 확인할 가상 키 코드 (Virtual Key Code)
   * @return 이번 프레임에 처음 눌렸다면 true 반환
   */
  bool GetKeyDown(int key_code);

  /**
   * @brief 키가 이번 프레임에 떼어졌는지 확인 (Up Event)
   * @param key_code 확인할 가상 키 코드 (Virtual Key Code)
   * @return 이번 프레임에 떼어졌다면 true 반환
   */
  bool GetKeyUp(int key_code);

  /**
   * @brief 키가 현재 눌려있는 상태인지 확인 (Hold State)
   * @param key_code 확인할 가상 키 코드 (Virtual Key Code)
   * @return 키가 눌려있는 상태라면 true 반환
   */
  bool GetKey(int key_code);

  /**
   * @brief Input 클래스의 싱글톤 인스턴스를 반환
   * @return Input 클래스의 인스턴스 참조
   */
  static Input& Get();

 private:
  Input();
  ~Input();

  /**
   * @brief WinAPI를 통해 실제 키보드 입력 상태를 읽어옴 (Engine에서 호출)
   */
  void ProcessInput();

  /**
   * @brief 현재 프레임의 입력 상태를 이전 프레임 상태로 복사 (Engine에서 호출)
   */
  void SavePreviousInputStates();

  KeyState key_states_[255] = {};  ///< 255개 가상 키의 상태 정보를 담는 배열

  static Input* instance_;  ///< 싱글톤 인스턴스 포인터
};

}  // namespace mint

#endif  // MINT_ENGINE_CORE_INPUT_H_
