#ifndef GUILD_MANAGER_UTIL_HERO_FACTORY_H_
#define GUILD_MANAGER_UTIL_HERO_FACTORY_H_

#include <string>
#include <vector>

#include "Common/GameTypes.h"

namespace guild {

class Hero;
class Enemy;

/**
 * @struct UnitData
 * @brief CSV에서 읽어온 원본 유닛 데이터를 저장하는 구조체.
 */
struct UnitData {
  std::wstring name;
  CharacterJob job;
  int hp;
  int atk;
  int speed;
  char grade;
  std::wstring image;
};

/**
 * @class HeroFactory
 * @brief CSV 데이터를 바탕으로 Hero 및 Enemy 객체를 생성하는 팩토리 클래스.
 */
class HeroFactory {
 public:
  /**
   * @brief CSV 파일에서 모든 유닛 데이터를 메모리로 로드한다.
   * @param path CSV 파일 경로.
   */
  static void LoadAllUnitData(const std::wstring& path);

  /**
   * @brief 로드된 데이터 풀에서 랜덤하게 Hero들을 생성한다.
   * @param count 생성할 개수.
   * @return 생성된 Hero 객체들의 벡터.
   */
  static std::vector<Hero*> CreateRandomHeroes(int count);

  /**
   * @brief 로드된 데이터 풀에서 랜덤하게 Enemy들을 생성한다. (랜덤 매칭용)
   * @param count 생성할 개수.
   * @return 생성된 Enemy 객체들의 벡터.
   */
  static std::vector<Enemy*> CreateRandomEnemies(int count);

  /**
   * @brief 특정 등급의 유닛을 랜덤하게 하나 생성하여 Enemy로 반환한다.
   * @param grade 찾고자 하는 등급 (L"S", L"A" 등).
   * @return 생성된 Enemy 객체 (없을 경우 nullptr).
   */
  static Enemy* CreateRandomEnemyByGrade(const std::wstring& grade);

  /**
   * @brief 특정 등급의 유닛을 랜덤하게 하나 생성하여 Hero로 반환한다.
   * @param grade 찾고자 하는 등급.
   * @return 생성된 Hero 객체.
   */
  static Hero* CreateRandomHeroByGrade(const std::wstring& grade);

  /**
   * @brief 구버전 호환용: CSV 파일에서 직접 Hero들을 생성한다.
   */
  static std::vector<Hero*> CreateHeroesFromCSV(const std::wstring& path);

  static size_t unit_pool_size() { return unit_pool_.size(); }

 private:
  /**
   * @brief 문자열을 CharacterJob 열거형으로 변환한다.
   */
  static CharacterJob StringToJob(const std::wstring& str);

  static std::vector<UnitData> unit_pool_;  ///< 로드된 모든 캐릭터 데이터 풀
};

}  // namespace guild

#endif  // GUILD_MANAGER_UTIL_HERO_FACTORY_H_
