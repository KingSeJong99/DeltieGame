/**
 * @file Logger.h
 * @brief 디버깅용 로그를 파일에 기록하는 싱글톤 클래스
 * 
 * [사용방법]
 * 1. 로그를 남기고 싶은 소스코드에 헤더파일을 포함시킨다.
 * 2. MINT_LOG_INFO, MINT_LOG_DEBUG, MINT_LOG_WARN, MINT_LOG_ERROR 매크로를 사용하여 로그를 남긴다.
 *    예: MINT_LOG_INFO(L"영웅 이름: " + actor->name() + L", 속도: " + std::to_wstring(actor->turn_speed()));
 * 3. PowerShell에서 'Get-Content debug.log -Wait -Tail 10 -Encoding utf8' 명령어로 실시간 모니터링한다.
 */

#ifndef MINT_ENGINE_CORE_LOGGER_H_
#define MINT_ENGINE_CORE_LOGGER_H_

#include <Windows.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 로그 위험도 수준
 */
enum class LogLevel { kInfo, kDebug, kWarning, kError };

/**
 * @brief 디버깅용 로그를 파일에 기록하는 싱글톤 클래스
 */
class MINT_API Logger {
 public:
  static Logger& Get() {
    static Logger instance;
    return instance;
  }

  /**
   * @brief 로그 기록 (std::string)
   */
  void Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!log_file_.is_open()) return;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) % 1000;

    struct tm time_info;
    localtime_s(&time_info, &in_time_t);

    log_file_ << "[" << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S") << "."
              << std::setfill('0') << std::setw(3) << ms.count() << "] "
              << "[" << GetLevelString(level) << "] " << message << "\n";
    log_file_.flush();
  }

  /**
   * @brief 로그 기록 (std::wstring - UTF-8로 변환하여 저장)
   */
  void Log(LogLevel level, const std::wstring& message) {
    if (message.empty()) return;

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, message.c_str(),
                                          (int)message.size(), NULL, 0, NULL, NULL);
    std::string utf8_message(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, message.c_str(), (int)message.size(),
                        &utf8_message[0], size_needed, NULL, NULL);

    Log(level, utf8_message);
  }

 private:
  Logger() { log_file_.open("debug.log", std::ios::out | std::ios::trunc); }
  ~Logger() {
    if (log_file_.is_open()) {
      log_file_.close();
    }
  }

  const char* GetLevelString(LogLevel level) const {
    switch (level) {
      case LogLevel::kInfo: return "INFO";
      case LogLevel::kDebug: return "DEBUG";
      case LogLevel::kWarning: return "WARN";
      case LogLevel::kError: return "ERROR";
      default: return "UNKNOWN";
    }
  }

  std::ofstream log_file_;
  std::mutex mutex_;
};

}  // namespace mint

// 편의용 매크로 정의
#define MINT_LOG_INFO(msg) mint::Logger::Get().Log(mint::LogLevel::kInfo, msg)
#define MINT_LOG_DEBUG(msg) mint::Logger::Get().Log(mint::LogLevel::kDebug, msg)
#define MINT_LOG_WARN(msg) mint::Logger::Get().Log(mint::LogLevel::kWarning, msg)
#define MINT_LOG_ERROR(msg) mint::Logger::Get().Log(mint::LogLevel::kError, msg)

#endif  // MINT_ENGINE_CORE_LOGGER_H_
