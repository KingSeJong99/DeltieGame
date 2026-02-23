#ifndef MINT_ENGINE_CORE_CPU_TIMER_H_
#define MINT_ENGINE_CORE_CPU_TIMER_H_

#include <Windows.h>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 고정밀 하드웨어 타이머를 사용하는 타이머 클래스
 */
class MINT_API CpuTimer {
 public:
  CpuTimer();
  ~CpuTimer() = default;

  // 프레임 간의 시간 차이 계산
  void Tick();

  // 타이머 초기화
  void Reset();

  // 이전 프레임과 현재 프레임 사이의 시간(초) 반환
  inline double GetDeltaTime() const { return delta_time_; }

  // 타이머가 시작된 이후 경과된 총 시간(초) 반환
  double GetTotalTime() const;

 private:
  LARGE_INTEGER frequency_ = {};
  LARGE_INTEGER previous_time_ = {};
  LARGE_INTEGER current_time_ = {};
  LARGE_INTEGER base_time_ = {};

  double delta_time_ = 0.0;
};

}  // namespace mint

#endif  // MINT_ENGINE_CORE_CPU_TIMER_H_
