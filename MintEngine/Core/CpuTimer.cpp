#include "MintEngine/Core/CpuTimer.h"

namespace mint {

CpuTimer::CpuTimer() : delta_time_(0.0) {
  QueryPerformanceFrequency(&frequency_);
  QueryPerformanceCounter(&current_time_);
  previous_time_ = current_time_;
  base_time_ = current_time_;
}

void CpuTimer::Tick() {
  previous_time_ = current_time_;
  QueryPerformanceCounter(&current_time_);

  delta_time_ = static_cast<double>(current_time_.QuadPart - previous_time_.QuadPart) /
                static_cast<double>(frequency_.QuadPart);
}

void CpuTimer::Reset() {
  QueryPerformanceCounter(&base_time_);
}

double CpuTimer::GetTotalTime() const {
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return static_cast<double>(now.QuadPart - base_time_.QuadPart) / 
         static_cast<double>(frequency_.QuadPart);
}

}  // namespace mint
