#ifndef MINT_ENGINE_CORE_CPU_TIMER_H_
#define MINT_ENGINE_CORE_CPU_TIMER_H_

#include <Windows.h>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 고정밀 하드웨어 카운터를 활용한 성능 측정 및 타이밍 관리 클래스
 */
class MINT_API CpuTimer {
 public:
  /**
   * @brief CpuTimer 생성자 (주파수 및 시간 정보 초기화)
   */
  CpuTimer();

  /**
   * @brief CpuTimer 소멸자
   */
  ~CpuTimer() = default;

  /**
   * @brief 프레임 간 경과 시간(Delta Time)을 계산하여 갱신
   */
  void Tick();

  /**
   * @brief 기준 시간(Base Time)을 현재 시점으로 초기화
   */
  void Reset();

  /**
   * @brief 이전 틱과 현재 틱 사이의 경과 시간(초 단위)을 반환
   * @return 계산된 델타 타임 (초 단위, double)
   */
  inline double GetDeltaTime() const { return delta_time_; }

  /**
   * @brief Reset() 호출 시점(또는 생성 시점) 이후 경과된 총 시간(초 단위)을 반환
   * @return 시작 시점부터 현재까지의 누적 시간 (초 단위, double)
   */
  double GetTotalTime() const;

 private:
  LARGE_INTEGER frequency_ = {};      ///< CPU 하드웨어 카운터 주파수
  LARGE_INTEGER previous_time_ = {};   ///< 이전 프레임의 카운터 값
  LARGE_INTEGER current_time_ = {};    ///< 현재 프레임의 카운터 값
  LARGE_INTEGER base_time_ = {};       ///< 총 시간 측정을 위한 기준 카운터 값

  double delta_time_ = 0.0;            ///< 계산된 프레임 간 경과 시간 (초)
};

}  // namespace mint

#endif  // MINT_ENGINE_CORE_CPU_TIMER_H_
