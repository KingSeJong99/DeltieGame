#ifndef GUILD_MANAGER_COMMON_GAME_TYPES_H_
#define GUILD_MANAGER_COMMON_GAME_TYPES_H_

#include <string>

namespace guild {

/**
 * @brief 캐릭터의 현재 행동 상태를 정의하는 열거형
 */
enum class CharacterState {
  kWaiting = 0, ///< 턴 대기 중
  kDeciding,    ///< 행동 결정 중
  kMoving,      ///< 이동 중
  kAttacking,   ///< 공격 중
};

/**
 * @enum CharacterJob
 * @brief 직업군을 정의하는 열거형 (메이플스토리 모험가 기준)
 */
enum class CharacterJob {
  kHero = 0,		///< 히어로
  kPaladin,			///< 팔라딘
  kDarkKnight,		///< 다크나이트
  kArchMageFire,	///< 아크메이지(불,독)
  kArchMageIce,		///< 아크메이지(썬,콜)
  kBishop,			///< 비숍
  kBowMaster,		///< 보우마스터
  kMarksman,		///< 신궁
  kNightLord,		///< 나이트로드
  kShadower,		///< 섀도어

  // 특수
  kBeginner,		/// 초보자
  kAdmin			/// 최강
};

/**
 * @brief CharacterJob 열거형 값을 한글 문자열로 변환한다.
 * @param job 변환할 직업 열거형 값.
 * @return 해당 직업의 한글 명칭 (wstring).
 */
inline std::wstring JobToWString(CharacterJob job) {
  switch (job) {
    case CharacterJob::kHero: return L"히어로";
    case CharacterJob::kPaladin: return L"팔라딘";
    case CharacterJob::kDarkKnight: return L"다크나이트";
    case CharacterJob::kArchMageFire: return L"아크메이지(불,독)";
    case CharacterJob::kArchMageIce: return L"아크메이지(썬,콜)";
    case CharacterJob::kBishop: return L"비숍";
    case CharacterJob::kBowMaster: return L"보우마스터";
    case CharacterJob::kMarksman: return L"신궁";
    case CharacterJob::kNightLord: return L"나이트로드";
    case CharacterJob::kShadower: return L"섀도어";
    case CharacterJob::kBeginner: return L"초보자";
    case CharacterJob::kAdmin: return L"관리자";
    default: return L"미확인";
  }
}

}  // namespace guild

#endif  // GUILD_MANAGER_COMMON_GAME_TYPES_H_
