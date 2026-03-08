/**
 * @file Logger.h
 * @brief 디버깅용 로그를 파일에 기록하는 싱글톤 클래스.
 *
 * [추가사항]
 * 특정 대상의 상태를 감시하거나 추적하기 위하여 기존의 로그 매크로를 분리하였다.
 * 예를들어, Level 단계에서 Actor를 수행하거나, Actor에서 자신의 상태를 조회할 때,
 * MINT_LOG_LEVEL_TAG(tag, msg) 를 사용하면 로그를 호출하는 함수 위치에서 관찰하고자 하는
 * 대상의 태그를 붙일 수 있다.
 * 
 * [사용방법]
 * 1. 로그를 남기고 싶은 소스코드에 헤더파일을 포함시킨다.
 * 2. MINT_LOG_INFO, MINT_LOG_DEBUG, MINT_LOG_WARN, MINT_LOG_ERROR 매크로를
 * 사용하여 로그를 남긴다. 예: MINT_LOG_INFO(L"영웅 이름: " + actor->name() +
 * L", 속도: " + std::to_wstring(actor->turn_speed()));
 * 3. PowerShell에서 'Get-Content debug.log -Wait -Tail 10 -Encoding utf8'
 * 명령어로 실시간 모니터링한다.
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
   * @brief 로그 기록 (std::string).
   */
  void Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!log_file_.is_open()) return;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    struct tm time_info;
    localtime_s(&time_info, &in_time_t);

    log_file_ << "[" << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S") << "."
              << std::setfill('0') << std::setw(3) << ms.count() << "] "
              << "[" << GetLevelString(level) << "] " << message << "\n";
    log_file_.flush();
  }

  /**
   * @brief 로그 기록 (std::wstring - UTF-8로 변환하여 저장).
   */
  void Log(LogLevel level, const std::wstring& message) {
    if (message.empty()) return;

    int size_needed = WideCharToMultiByte(
        CP_UTF8, 0, message.c_str(), (int)message.size(), NULL, 0, NULL, NULL);
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
      case LogLevel::kInfo:
        return "INFO";
      case LogLevel::kDebug:
        return "DEBUG";
      case LogLevel::kWarning:
        return "WARN";
      case LogLevel::kError:
        return "ERROR";
      default:
        return "UNKNOWN";
    }
  }

  std::ofstream log_file_;
  std::mutex mutex_;
};

}  // namespace mint

// 1. 기본 로그 (태그 없이 위치 정보만 포함 - 시스템 전용)
#define MINT_LOG_INFO(msg)                       \
  mint::Logger::Get().Log(mint::LogLevel::kInfo, \
                          std::wstring(L"[") + __FUNCTIONW__ + L"] " + msg)

#define MINT_LOG_DEBUG(msg)                       \
  mint::Logger::Get().Log(mint::LogLevel::kDebug, \
                          std::wstring(L"[") + __FUNCTIONW__ + L"] " + msg)

#define MINT_LOG_WARN(msg)                          \
  mint::Logger::Get().Log(mint::LogLevel::kWarning, \
                          std::wstring(L"[") + __FUNCTIONW__ + L"] " + msg)

#define MINT_LOG_ERROR(msg)                       \
  mint::Logger::Get().Log(mint::LogLevel::kError, \
                          std::wstring(L"[") + __FUNCTIONW__ + L"] " + msg)

// 2. 태그 포함 로그 (특정 액터나 대상을 추적할 때 사용 - 엔티티 전용)
#define MINT_LOG_INFO_TAG(tag, msg) \
  mint::Logger::Get().Log(          \
      mint::LogLevel::kInfo,        \
      std::wstring(L"[") + __FUNCTIONW__ + L"] [" + tag + L"] " + msg)

#define MINT_LOG_DEBUG_TAG(tag, msg) \
  mint::Logger::Get().Log(           \
      mint::LogLevel::kDebug,        \
      std::wstring(L"[") + __FUNCTIONW__ + L"] [" + tag + L"] " + msg)

#define MINT_LOG_WARN_TAG(tag, msg) \
  mint::Logger::Get().Log(          \
      mint::LogLevel::kWarning,     \
      std::wstring(L"[") + __FUNCTIONW__ + L"] [" + tag + L"] " + msg)

#define MINT_LOG_ERROR_TAG(tag, msg) \
  mint::Logger::Get().Log(           \
      mint::LogLevel::kError,        \
      std::wstring(L"[") + __FUNCTIONW__ + L"] [" + tag + L"] " + msg)

#endif  // MINT_ENGINE_CORE_LOGGER_H_
