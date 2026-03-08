#ifndef MINT_ENGINE_ENGINE_ENGINE_H_
#define MINT_ENGINE_ENGINE_ENGINE_H_

#include <Windows.h>

#include "MintEngine/Core/Common.h"
#include "MintEngine/Core/RTTI.h"
#include "MintEngine/Math/Vector2.h"

namespace mint {

class Input;
class Renderer;
class Level;

/**
 * @class Engine
 * @brief MintEngine의 핵심 시스템을 관리하는 메인 엔진 클래스.
 * 
 * @note 엔진의 초기화, 메인 루프(Run), 자원 해제 및 시스템 간의 중재를 담당한다.
 * RTTI를 상속받으며 싱글톤 패턴으로 구현되어 있다.
 */
class MINT_API Engine : public RTTI {
  RTTI_DECLARATIONS(Engine, RTTI)

 public:
  /**
   * @struct EngineSetting
   * @brief 엔진 실행 시 필요한 기본 설정값들을 담는 구조체
   */
  struct EngineSetting {
    float framerate = 0.0f;  ///< 초당 프레임 제한(0.0f일 경우 기본값 60.0f 사용)
    int width = 0;           ///< 콘솔 렌더링 윈도우의 가로 해상도
    int height = 0;          ///< 콘솔 렌더링 윈도우의 세로 해상도
  };

  Engine();
  virtual ~Engine();

  /**
   * @brief 엔진의 메인 루프를 실행한다.
   * 
   * @note BeginPlay를 호출한 뒤, is_quit_이 true가 될 때까지 Tick과 Draw를 반복한다
   */
  void Run();

  /**
   * @brief 엔진 종료 플래그를 활성화한다.
   * 
   * @note 현재 프레임이 종료된 후 메인 루프가 정지된다.
   */
  void QuitEngine();

  /**
   * @brief 엔진에 새로운 레벨(Scene)을 설정한다.
   * @param new_level 새로 교체할 레벨 객체의 포인터. (기존 레벨은 내부에서 삭제됨)
   */
  void SetNewLevel(Level* new_level);

  /**
   * @brief 엔진의 싱글톤 인스턴스를 반환한다.
   * @return Engine& 엔진 인스턴스의 참조자.
   */
  static Engine& Get();

 protected:
  /**
   * @brief 지정된 버퍼를 공백 문자로 초기화한다.
   * @param buffer 초기화할 CHAR_INFO 버퍼.
   * @param width 버퍼의 가로 크기.
   * @param height 버퍼의 세로 크기.
   */
  void Clear(CHAR_INFO* buffer, int width, int height);

  /**
   * @brief 엔진 종료 시 호출되어 자원을 정리한다.
   */
  void Shutdown();

  /**
   * @brief 외부 파일(Config/Setting.txt)로부터 엔진 설정을 로드한다.
   */
  void LoadSetting();

  /**
   * @brief 게임 시작 시 최초 1회 실행되는 이벤트 함수.
   */
  virtual void BeginPlay();

  /**
   * @brief 매 프레임 로직 업데이트를 수행하는 함수.
   * @param delta_time 이전 프레임부터 현재 프레임까지의 경과시간(초).
   */
  virtual void Tick(float delta_time);

  /**
   * @biref 매 프레임 렌더링을 수행하는 함수.
   */
  virtual void Draw();

  bool is_quit_ = false;          ///< 엔진의 종료 여부 플래그
  EngineSetting setting_;         ///< 엔진의 설정 정보
  Input* input_ = nullptr;        ///< 입력 시스템 포인터
  Renderer* renderer_ = nullptr;  ///< 렌더링 시스템 포인터
  Vector2 screen_size_;           ///< 화면의 실제 크기(Vector2 타입)
  Level* main_level_ = nullptr;   ///< 현재 실행 중인 레벨 객체

  static Engine* instance_;       ///< 싱글톤 인스턴스 포인터
};

}  // namespace mint

#endif  // MINT_ENGINE_ENGINE_ENGINE_H_
