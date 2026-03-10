#ifndef GUILD_MANAGER_UTIL_GACHA_MODULE_H_
#define GUILD_MANAGER_UTIL_GACHA_MODULE_H_

#include <random>
#include <string>
#include <vector>

namespace guild {

/**
 * @struct GachaGrade
 * @brief 가챠 시스템에서 등급과 해당 등급이 뽑힐 가중치를 정의한다.
 */
struct GachaGrade {
  std::wstring name;  ///< 등급 명칭 (예: L"S", L"A" 등)
  int weight;         ///< 가중치 (전체 가중치 대비 비율)
};

/**
 * @class GachaModule
 * @brief 가중치 기반 랜덤 추출 및 천장 시스템을 처리하는 모듈.
 * @note 프로젝트 전반의 캐릭터 획득 로직을 담당한다.
 */
class GachaModule {
 public:
  /**
   * @brief 가챠 모듈 생성자. 기본 등급 세팅 및 가중치를 초기화한다.
   * @todo 나중에 JSON이나 CSV를 통해 외부에서 가중치 설정을 로드하도록 개선 필요.
   */
  GachaModule() : gen_(rd_()), pity_count_(0), total_weight_(0) {
    // 기본 등급 세팅 (S: 1%, A: 10%, B: 30%, C: 59% 가정)
    grades_ = {{L"S", 10}, {L"A", 100}, {L"B", 300}, {L"C", 590}};

    for (const auto& grade : grades_) {
      total_weight_ += grade.weight;
    }
  }

  ~GachaModule() = default;

  /**
   * @brief 가챠를 1회 수행하여 결정된 등급을 반환한다.
   * @return 뽑힌 등급 문자열 (L"S", L"A" 등).
   * @note 천장 스택(pity_count_)을 관리하며, S등급 획득 시 초기화한다.
   */
  std::wstring Draw() {
    ++pity_count_;

    // 1. 천장 확인 (kPityThreshold회 뽑기 동안 S가 안 나오면 강제 지급)
    if (pity_count_ >= kPityThreshold) {
      pity_count_ = 0;
      return L"S";
    }

    // 2. 가중치 기반 무작위 추출
    std::uniform_int_distribution<int> dis(0, total_weight_ - 1);
    int pick = dis(gen_);

    int current_sum = 0;
    for (const auto& grade : grades_) {
      current_sum += grade.weight;

      if (pick < current_sum) {
        // S등급을 뽑았다면 천장 스택 초기화
        if (grade.name == L"S") {
          pity_count_ = 0;
        }
        return grade.name;
      }
    }

    return L"C";  // 기본값 (안전 장치)
  }

  /** @brief 현재 쌓인 천장 스택을 반환한다. */
  inline int pity_count() const { return pity_count_; }

 private:
  static constexpr int kPityThreshold = 200;  ///< 천장 기준 횟수

  std::vector<GachaGrade> grades_;  ///< 등급 데이터 리스트
  int pity_count_;                  ///< 현재 누적 천장 스택
  int total_weight_;                ///< 전체 가중치 합계

  std::random_device rd_;
  std::mt19937_64 gen_;  ///< 64비트 메르센 트위스터 생성기
};

}  // namespace guild

#endif  // GUILD_MANAGER_UTIL_GACHA_MODULE_H_
