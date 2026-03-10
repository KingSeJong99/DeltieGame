#include "HeroFactory.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "Actor/Enemy.h"
#include "Actor/Hero.h"
#include "MintEngine/Core/Util.h"

namespace guild {

// 정적 멤버 변수 초기화
std::vector<UnitData> HeroFactory::unit_pool_;

void HeroFactory::LoadAllUnitData(const std::wstring& path) {
  unit_pool_.clear();

  std::ifstream file(path);
  if (!file.is_open()) return;

  std::string line;
  if (!std::getline(file, line)) return;

  while (std::getline(file, line)) {
    if (line.empty()) continue;

    std::wstring wline = mint::util::UTF8ToWide(line);
    wline = mint::util::Trim(wline);
    if (wline.empty()) continue;

    auto tokens = mint::util::Split(wline, L',');
    if (tokens.size() < 7) continue;

    try {
      UnitData data;
      data.name = tokens[0];
      data.job = StringToJob(tokens[1]);
      data.hp = std::stoi(tokens[2]);
      data.atk = std::stoi(tokens[3]);
      data.speed = std::stoi(tokens[4]);
      data.grade = tokens[5].empty() ? 'F' : (char)tokens[5][0];
      data.image = tokens[6];

      unit_pool_.push_back(data);
    } catch (...) {
      continue;
    }
  }
  file.close();
}

std::vector<Hero*> HeroFactory::CreateRandomHeroes(int count) {
  std::vector<Hero*> result;
  if (unit_pool_.empty()) return result;

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, (int)unit_pool_.size() - 1);

  for (int i = 0; i < count; ++i) {
    const auto& data = unit_pool_[dis(gen)];
    result.push_back(new Hero(data.name, data.job, data.hp, data.atk, data.speed,
                              data.grade, data.image));
  }
  return result;
}

std::vector<Enemy*> HeroFactory::CreateRandomEnemies(int count) {
  std::vector<Enemy*> result;
  if (unit_pool_.empty()) return result;

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, (int)unit_pool_.size() - 1);

  for (int i = 0; i < count; ++i) {
    const auto& data = unit_pool_[dis(gen)];
    // 적군 생성! (Hero와 똑같은 데이터를 주되 Enemy 클래스로 인스턴스화)
    result.push_back(new Enemy(data.name, data.job, data.hp, data.atk,
                               data.speed, data.grade, data.image));
  }
  return result;
}

Enemy* HeroFactory::CreateRandomEnemyByGrade(const std::wstring& grade) {
  if (unit_pool_.empty() || grade.empty()) return nullptr;

  std::vector<const UnitData*> filtered;
  char target_grade = (char)grade[0];

  for (const auto& data : unit_pool_) {
    if (data.grade == target_grade) filtered.push_back(&data);
  }

  if (filtered.empty()) return nullptr;

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, (int)filtered.size() - 1);

  const auto* data = filtered[dis(gen)];
  return new Enemy(data->name, data->job, data->hp, data->atk, data->speed,
                   data->grade, data->image);
}

Hero* HeroFactory::CreateRandomHeroByGrade(const std::wstring& grade) {
  if (unit_pool_.empty() || grade.empty()) return nullptr;

  std::vector<const UnitData*> filtered;
  char target_grade = (char)grade[0];

  for (const auto& data : unit_pool_) {
    if (data.grade == target_grade) filtered.push_back(&data);
  }

  if (filtered.empty()) return nullptr;

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, (int)filtered.size() - 1);

  const auto* data = filtered[dis(gen)];
  return new Hero(data->name, data->job, data->hp, data->atk, data->speed,
                  data->grade, data->image);
}

std::vector<Hero*> HeroFactory::CreateHeroesFromCSV(const std::wstring& path) {
  // 만약 풀이 비어있으면 로드 먼저 수행
  if (unit_pool_.empty()) {
    LoadAllUnitData(path);
  }

  std::vector<Hero*> result;
  for (const auto& data : unit_pool_) {
    result.push_back(new Hero(data.name, data.job, data.hp, data.atk, data.speed,
                              data.grade, data.image));
  }
  return result;
}

CharacterJob HeroFactory::StringToJob(const std::wstring& str) {
  if (str == L"Hero") return CharacterJob::kHero;
  if (str == L"Paladin") return CharacterJob::kPaladin;
  if (str == L"DarkKnight") return CharacterJob::kDarkKnight;
  if (str == L"ArchMageFire") return CharacterJob::kArchMageFire;
  if (str == L"ArchMageIce") return CharacterJob::kArchMageIce;
  if (str == L"Bishop") return CharacterJob::kBishop;
  if (str == L"BowMaster") return CharacterJob::kBowMaster;
  if (str == L"Marksman") return CharacterJob::kMarksman;
  if (str == L"NightLord") return CharacterJob::kNightLord;
  if (str == L"Shadower") return CharacterJob::kShadower;
  if (str == L"Admin") return CharacterJob::kAdmin;

  return CharacterJob::kBeginner;
}

}  // namespace guild
