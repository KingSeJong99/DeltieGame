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
 * @brief 게임 엔진의 메인 클래스
 */
class MINT_API Engine : public RTTI {
  RTTI_DECLARATIONS(Engine, RTTI)

 public:
  // 엔진 설정 구조체
  struct EngineSetting {
    float framerate = 0.0f;
    int width = 0;
    int height = 0;
  };

  Engine();
  virtual ~Engine();

  // 엔진의 메인 루프 실행
  void Run();

  // 엔진 종료 요청
  void QuitEngine();

  // 새로운 레벨 설정
  void SetNewLevel(Level* new_level);

  // 싱글톤 인스턴스 반환
  static Engine& Get();

 protected:
  void Clear(CHAR_INFO* buffer, int width, int height);

  // 엔진 자원 정리
  void Shutdown();

  // 설정 로드
  void LoadSetting();

  // 초기화 이벤트
  virtual void BeginPlay();

  // 업데이트 이벤트
  virtual void Tick(float delta_time);

  // 그리기 이벤트
  virtual void Draw();

  bool is_quit_ = false;
  EngineSetting setting_;
  Input* input_ = nullptr;
  Renderer* renderer_ = nullptr;
  Vector2 screen_size_;
  Level* main_level_ = nullptr;

  static Engine* instance_;
};

}  // namespace mint

#endif  // MINT_ENGINE_ENGINE_ENGINE_H_
