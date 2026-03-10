#include "GuildLevel.h"

#include <algorithm>
#include <string>

#include "Actor/Hero.h"
#include "Level/BattleLevel.h"
#include "MintEngine/Core/Color.h"
#include "MintEngine/Core/Input.h"
#include "MintEngine/Core/Logger.h"
#include "MintEngine/Engine/Engine.h"
#include "MintEngine/Render/Renderer.h"
#include "MintUI/TextLayout.h"
#include "Util/GachaModule.h"  // 추가: 가챠 시스템
#include "Util/HeroFactory.h"

namespace guild {

GuildLevel::GuildLevel() : selected_index_(0) {
  // [CLI 수정 반영]: 가챠 모듈 인스턴스화
  // 이유: 원본 헤더에 추가된 GachaModule 객체를 실제로 사용하기 위해 메모리
  // 할당이 필요합니다.
  gacha_module_ = new GachaModule();
}

GuildLevel::~GuildLevel() {
  if (ui_layout_) {
    delete ui_layout_;
    ui_layout_ = nullptr;
  }
  // [CLI 수정 반영]: 메모리 누수 방지
  if (gacha_module_) {
    delete gacha_module_;
    gacha_module_ = nullptr;
  }
}

void GuildLevel::BeginPlay() {
  MINT_LOG_INFO(L"레벨을 불러오고 있어요.");
  // 원본 로직 유지: CSV 데이터 로드
  HeroFactory::CreateHeroesFromCSV(L"Config/Heroes.csv");

  if (HeroFactory::unit_pool_size() == 0) {
    MINT_LOG_ERROR(
        L"데이터 파일(Config/Heroes.csv)이 비어있거나 찾을 수 없습니다.");
    __debugbreak();
    return;
  }

  // 3. 플레이어에게 기초 3명 지급!
  for (int i = 0; i < 3; ++i) {
    Hero* starter = HeroFactory::CreateRandomHeroByGrade(L"C");
    if (starter) {
      data_.owned_heroes().push_back(starter);
    }
  }

  // 4. 지급 완료 후 로그 출력
  MINT_LOG_INFO(L"본부 세팅 완료. 현재 가용 용사: " +
                std::to_wstring(data_.owned_heroes().size()) + L"명.");
}

void GuildLevel::Tick(float delta_time) {
  int hero_count = static_cast<int>(data_.owned_heroes().size());

  // 1. 가챠 연출 처리 (애니메이션 중이면 다른 입력 차단)
  if (is_animating_) {
    gacha_timer_ += delta_time;
    if (gacha_timer_ >= 0.2f) {  // 0.2초마다 하나씩 공개
      gacha_timer_ = 0.0f;
      reveal_count_++;
      if (reveal_count_ >= last_pull_results_.size()) {
        is_animating_ = false;  // 모든 결과 공개 완료
      }
    }
    return;  // 연출 중에는 아래의 입력 로직을 실행하지 않음
  }

  // 2. 상태 전환 (Tab 키)
  if (mint::Input::Get().GetKeyDown(VK_TAB)) {
    current_state_ = (current_state_ == GuildState::kManagement)
                         ? GuildState::kGacha
                         : GuildState::kManagement;

    MINT_LOG_INFO(L"화면 전환: " +
                  std::wstring(current_state_ == GuildState::kGacha
                                   ? L"가챠 모드"
                                   : L"관리 모드"));
  }

  // 3. [관리 모드] 입력 처리
  if (current_state_ == GuildState::kManagement) {
    // 리스트 이동
    if (hero_count > 0) {
      if (mint::Input::Get().GetKeyDown(VK_DOWN)) {
        selected_index_ = (selected_index_ + 1) % hero_count;
      }
      if (mint::Input::Get().GetKeyDown(VK_UP)) {
        selected_index_ = (selected_index_ - 1 + hero_count) % hero_count;
      }
    }

    // 파티 편성 (Space 키)
    if (mint::Input::Get().GetKeyDown(VK_SPACE) && hero_count > 0) {
      Hero* selected_hero = data_.owned_heroes()[selected_index_];
      auto it =
          std::find(data_.party().begin(), data_.party().end(), selected_hero);

      if (it != data_.party().end()) {
        MINT_LOG_INFO_TAG(selected_hero->name(), L"파티에서 제거 했습니다.");
        data_.party().erase(it);
      } else {
        if (data_.party().size() < kMaxPartySize) {
          MINT_LOG_INFO_TAG(selected_hero->name(), L"파티에 추가 했습니다.");
          data_.party().push_back(selected_hero);
        } else {
          MINT_LOG_WARN(L"파티가 이미 가득 찼습니다!");
        }
      }
    }

    // 전투 시작 (Enter 키)
    if (mint::Input::Get().GetKeyDown(VK_RETURN)) {
      if (data_.party().size() == kMaxPartySize) {
        for (Hero* hero : data_.party()) {
          RemoveActorWithoutDeleting(hero);
        }
        BattleLevel* battle_level = new BattleLevel(data_.party());
        mint::Engine::Get().SetNewLevel(battle_level);
      } else {
        MINT_LOG_WARN(L"파티원이 부족합니다! (필요: " +
                      std::to_wstring(kMaxPartySize) + L")");
      }
    }
  }
  // 4. [가챠 모드] 입력 처리
  else if (current_state_ == GuildState::kGacha) {
    // [1] 키: 10연차 (900G)
    if (mint::Input::Get().GetKeyDown('1')) {
      if (data_.SpendMoney(900)) {
        last_pull_results_.clear();
        reveal_count_ = 0;
        is_animating_ = true;

        for (int i = 0; i < 10; ++i) {
          std::wstring grade = gacha_module_->Draw();
          Hero* h = HeroFactory::CreateRandomHeroByGrade(grade);
          if (h) {
            last_pull_results_.push_back(h);
            data_.owned_heroes().push_back(h);
          }
        }
        MINT_LOG_INFO(L"10연차 소환 시작!");
      } else {
        MINT_LOG_WARN(L"900G가 없네? 바보야, 돈부터 벌어와!");
      }
    }

    // [G] 키: 1회차 (100G)
    if (mint::Input::Get().GetKeyDown('G')) {
      if (data_.SpendMoney(100)) {
        last_pull_results_.clear();
        reveal_count_ = 0;
        is_animating_ = true;

        std::wstring grade = gacha_module_->Draw();
        Hero* h = HeroFactory::CreateRandomHeroByGrade(grade);
        if (h) {
          last_pull_results_.push_back(h);
          data_.owned_heroes().push_back(h);
          MINT_LOG_INFO_TAG(h->name(), L"소환 성공! 등급: " + grade);
        }
      } else {
        MINT_LOG_WARN(L"돈이 부족합니다!");
      }
    }
  }
}

void GuildLevel::Draw(mint::Renderer& renderer, int width, int height) {
  if (!ui_layout_) {
    ui_layout_ = new mint::ui::TextLayout(renderer);
  }

  if (current_state_ == GuildState::kManagement) {
    DrawUI(renderer);

  } else {
    DrawGachaUI(renderer);
  }

  Level::Draw(renderer, width, height);
}

void GuildLevel::DrawUI(mint::Renderer& renderer) {
  // 1. 전체 프레임 (150x40 기준)
  ui_layout_->DrawBox(5, 2, 150, 38, L" GUILD HEADQUARTERS - ADMIN: SIEL ",
                      mint::Color::kCyan);

  // 2. 상단 정보 바 (돈, 보유 용사 수, 탭 전환 팁)
  // GameDataManager(data_)를 통해 실시간 세이브 데이터를 가져와 출력합니다.
  std::wstring top_info = L" [ GUILD MAIN ]   Messo: " +
                          std::to_wstring(data_.money()) + L"M   Heroes: " +
                          std::to_wstring(data_.owned_heroes().size()) +
                          L" / 120    [Tab to Switch Summoning Altar]";
  ui_layout_->DrawTextAligned(7, 4, 146, top_info, mint::ui::Alignment::Left,
                              mint::Color::kWhite);

  // 3. 구역별 렌더링 호출
  // 단일 함수에 모든 렌더링을 넣지 않고 역할별로 쪼개어 가독성을 높였습니다.
  DrawHeroList(renderer, 7, 6, 45, 24);      // 용사 인벤토리 (왼쪽)
  DrawHeroDetail(renderer, 54, 6, 99, 24);   // 상세 정보창 (오른쪽)
  DrawPartyStatus(renderer, 7, 31, 146, 5);  // 파티 편성창 (하단)

  // 4. 최하단 조작 가이드 (Key Guide)
  // 파티원 수에 따라 동적으로 가이드를 변경하여 플레이어의 편의성을 높였습니다.
  if (data_.party().size() == static_cast<size_t>(kMaxPartySize)) {
    ui_layout_->DrawTextAligned(
        7, 37, 146, L"★ 파티 편성 완료! [ENTER]를 눌러 전투를 시작하십시오! ★",
        mint::ui::Alignment::Center, mint::Color::kBrightGreen);
  } else {
    ui_layout_->DrawTextAligned(
        7, 37, 146,
        L"[↑↓]: 리스트 이동  [SPACE]: 파티 편성/해제  (필요: " +
            std::to_wstring(kMaxPartySize) + L"명)",
        mint::ui::Alignment::Center, mint::Color::kGray);
  }
}

void GuildLevel::DrawHeroList(mint::Renderer& renderer, int x, int y, int w,
                              int h) {
  // 인벤토리 박스 배경
  ui_layout_->DrawBox(x, y, w, h, L" HERO INVENTORY ", mint::Color::kWhite);

  if (data_.owned_heroes().empty()) {
    renderer.Submit(
        L"용사가 한 명도 없습니다.",
        {static_cast<float>(x) + 5.0f, static_cast<float>(y) + 5.0f},
        mint::Color::kGray);
    return;
  }

  // 1. 페이징 로직 (한 화면에 15명씩 출력)
  int page_size = 15;
  int start_idx = (selected_index_ / page_size) * page_size;

  for (int i = 0;
       i < page_size &&
       (start_idx + i) < static_cast<int>(data_.owned_heroes().size());
       ++i) {
    int idx = start_idx + i;
    Hero* hero = data_.owned_heroes()[idx];
    bool is_selected = (idx == selected_index_);
    bool is_in_party = data_.IsInParty(hero);

    // 2. 문자열 조립 (포인터 덧셈 에러 방지 버전!)
    std::wstring line = (is_selected ? L"> " : L"  ");
    line += (is_in_party ? L"[P] " : L"    ");
    line += L"[";
    line += static_cast<wchar_t>(hero->grade());  // 등급 한 글자
    line += L"] ";
    line += hero->name();

    // 3. 등급별 색상 결정
    mint::Color text_color = mint::Color::kWhite;
    if (hero->grade() == 'S') {
      text_color = mint::Color::kBrightCyan;
    } else if (hero->grade() == 'A') {
      text_color = mint::Color::kYellow;
    } else if (hero->grade() == 'C') {
      text_color = mint::Color::kGray;
    }

    // 4. 렌더링 제출 (선택된 항목은 반전 색상으로 강조!)
    renderer.Submit(
        line, {static_cast<float>(x) + 2.0f, static_cast<float>(y) + 2.0f + i},
        is_selected ? mint::Color::kBlack : text_color,
        is_selected ? text_color : mint::Color::kBlack, 200);
  }

  // 5. 페이지 정보 표시 (하단)
  int total_pages =
      (static_cast<int>(data_.owned_heroes().size()) + page_size - 1) /
      page_size;
  int current_page = (selected_index_ / page_size) + 1;
  std::wstring page_info = L"(Page " + std::to_wstring(current_page) + L" / " +
                           std::to_wstring(total_pages) + L")";

  renderer.Submit(page_info,
                  {static_cast<float>(x) + (w / 2.0f) - 5.0f,
                   static_cast<float>(y) + h - 1.0f},
                  mint::Color::kGray);
}

void GuildLevel::DrawHeroDetail(mint::Renderer& renderer, int x, int y, int w,
                                int h) {
  // 상세 정보 박스 배경
  ui_layout_->DrawBox(x, y, w, h, L" UNIT INSPECTION ",
                      mint::Color::kBrightCyan);

  // 1. 용사가 없거나 인덱스가 범위를 벗어나면 안내 메시지 출력
  if (data_.owned_heroes().empty() ||
      selected_index_ >= static_cast<int>(data_.owned_heroes().size())) {
    renderer.Submit(
        L"용사를 선택해 보십시오.",
        {static_cast<float>(x) + 10.0f, static_cast<float>(y) + 10.0f},
        mint::Color::kGray);
    return;
  }

  // 2. 선택된 용사 객체 가져오기
  Hero* hero = data_.owned_heroes()[selected_index_];

  // 3. 상단 텍스트 정보 (이름, 직업, 등급)
  renderer.Submit(L"이름: " + hero->name(),
                  {static_cast<float>(x) + 4.0f, static_cast<float>(y) + 3.0f},
                  mint::Color::kWhite);
  renderer.Submit(L"직업: " + JobToWString(hero->job()),
                  {static_cast<float>(x) + 4.0f, static_cast<float>(y) + 5.0f},
                  mint::Color::kWhite);

  // 등급 강조 (S: Cyan, A: Yellow, B/C: White)
  std::wstring grade_text =
      L"등급: [ ★ " + std::wstring(1, static_cast<wchar_t>(hero->grade())) +
      L" ★ ]";
  mint::Color grade_color = mint::Color::kWhite;
  if (hero->grade() == 'S') {
    grade_color = mint::Color::kBrightCyan;
  } else if (hero->grade() == 'A') {
    grade_color = mint::Color::kYellow;
  }

  renderer.Submit(grade_text,
                  {static_cast<float>(x) + 4.0f, static_cast<float>(y) + 7.0f},
                  grade_color);

  // 4. 능력치 시각화 (ProgressBar 활용)
  // 최대 능력치 설정 (HP: 10000, ATK: 1000, SPD: 200 가정)
  auto draw_stat = [&](int row, const std::wstring& label, int val, int max_val,
                       mint::Color color) {
    renderer.Submit(label,
                    {static_cast<float>(x) + 4.0f,
                     static_cast<float>(y) + static_cast<float>(row)},
                    mint::Color::kWhite);

    float ratio = (max_val > 0) ? std::clamp(static_cast<float>(val) / max_val,
                                             0.0f, 1.0f)
                                : 0.0f;

    ui_layout_->DrawProgressBar(x + 12, y + row, 40, 1, ratio, L'■', color,
                                mint::Color::kDarkGray, 200);

    renderer.Submit(std::to_wstring(val),
                    {static_cast<float>(x) + 54.0f,
                     static_cast<float>(y) + static_cast<float>(row)},
                    color);
  };

  // 능력치 바 렌더링 호출
  draw_stat(10, L"HP ", hero->max_hp(), 10000, mint::Color::kGreen);
  draw_stat(12, L"ATK", hero->atk(), 1000, mint::Color::kRed);
  draw_stat(14, L"SPD", hero->speed(), 200, mint::Color::kCyan);

  // 5. 파티 편성 여부 표시 (하단)
  if (data_.IsInParty(hero)) {
    renderer.Submit(
        L"★ 현재 파티에 편성된 용사입니다. ★",
        {static_cast<float>(x) + 4.0f, static_cast<float>(y) + 20.0f},
        mint::Color::kBrightGreen);
  } else {
    renderer.Submit(
        L"파티에 편성되지 않은 용사입니다.",
        {static_cast<float>(x) + 4.0f, static_cast<float>(y) + 20.0f},
        mint::Color::kGray);
  }
}

void GuildLevel::DrawPartyStatus(mint::Renderer& renderer, int x, int y, int w,
                                 int h) {
  // 파티 현황 박스 배경 (초록색 테두리로 전투 준비 완료 느낌!)
  ui_layout_->DrawBox(x, y, w, h, L" SELECTED PARTY STATUS ",
                      mint::Color::kGreen);

  const std::vector<Hero*>& party = data_.party();

  // 1. 파티 슬롯 3개를 가로로 나란히 배치 (너비 146을 3등분)
  for (int i = 0; i < kMaxPartySize; ++i) {
    float slot_x =
        static_cast<float>(x) + 2.0f + (i * 48.0f);  // 슬롯 시작 X 좌표
    float slot_y = static_cast<float>(y) + 2.0f;     // 슬롯 시작 Y 좌표

    if (i < static_cast<int>(party.size())) {
      // --- [슬롯에 용사가 있을 때] ---
      Hero* hero = party[i];

      // 등급별 색상 결정
      mint::Color text_color = mint::Color::kWhite;
      if (hero->grade() == 'S') {
        text_color = mint::Color::kBrightCyan;
      } else if (hero->grade() == 'A') {
        text_color = mint::Color::kYellow;
      }

      // 슬롯 번호와 용사 정보 표시
      std::wstring info = L"Slot " + std::to_wstring(i + 1) + L": [" +
                          static_cast<wchar_t>(hero->grade()) + L"] " +
                          hero->name();
      renderer.Submit(info, {slot_x, slot_y}, text_color, mint::Color::kBlack,
                      200);

      // 직업 정보 표시
      renderer.Submit(L"   (" + JobToWString(hero->job()) + L")",
                      {slot_x, slot_y + 1.0f}, mint::Color::kGray);
    } else {
      // --- [슬롯이 비어있을 때] ---
      std::wstring empty_info =
          L"Slot " + std::to_wstring(i + 1) + L": (비어 있음)";
      renderer.Submit(empty_info, {slot_x, slot_y}, mint::Color::kDarkGray,
                      mint::Color::kBlack, 200);
      renderer.Submit(L"   [ - ]", {slot_x, slot_y + 1.0f},
                      mint::Color::kDarkGray);
    }

    // 슬롯 구분선 (마지막 슬롯 제외)
    if (i < kMaxPartySize - 1) {
      for (int line = 1; line < 4; ++line) {
        renderer.Submit(L"│", {slot_x + 45.0f, static_cast<float>(y) + line},
                        mint::Color::kDarkGray);
      }
    }
  }
}

void GuildLevel::DrawGachaUI(mint::Renderer& renderer) {
  // 1. 전체 프레임 (마젠타 테마)
  ui_layout_->DrawBox(5, 2, 150, 38, L" GUILD SUMMONING ALTAR ",
                      mint::Color::kMagenta);

  // 2. 상단 정보 (돈, 보유량, 탭 전환)
  // GameDataManager를 통해 실시간 데이터를 반영하도록 유추함
  std::wstring top_info = L" [ GUILD MAIN ]   Messo: " +
                          std::to_wstring(data_.money()) + L"M   Heroes: " +
                          std::to_wstring(data_.owned_heroes().size()) +
                          L"/120   [Tab to Switch Management]";
  ui_layout_->DrawTextAligned(7, 4, 146, top_info, mint::ui::Alignment::Left,
                              mint::Color::kWhite);

  // 3. 중앙 연출 구역
  if (last_pull_results_.empty()) {
    // --- [대기 상태] 가챠 전 대기 연출 ---
    renderer.Submit(L".      .      * .           .           .", {40, 10},
                    mint::Color::kGray);
    renderer.Submit(L".         .             .      .          .", {35, 12},
                    mint::Color::kGray);

    // 슬롯 5개 (물음표 표시)
    ui_layout_->DrawTextAligned(
        7, 15, 146, L"[ ? ]   [ ? ]   [ ? ]   [ ? ]   [ ? ]",
        mint::ui::Alignment::Center, mint::Color::kBrightCyan);

    ui_layout_->DrawTextAligned(7, 20, 146, L"★ DESTINY IS CALLING YOU ★",
                                mint::ui::Alignment::Center,
                                mint::Color::kYellow);
    ui_layout_->DrawTextAligned(
        7, 22, 146, L"(Press 'G' for 1x or '1' for 10x Summon!)",
        mint::ui::Alignment::Center, mint::Color::kGray);
  } else {
    // --- [연출 상태] 가챠 결과 공개 (1장 또는 10장) ---
    // reveal_count_를 통해 애니메이션 효과를 부여함
    for (int i = 0;
         i < reveal_count_ && i < static_cast<int>(last_pull_results_.size());
         ++i) {
      Hero* h = last_pull_results_[i];

      // 등급별 가시성 확보 (S: Cyan, A: Yellow, C: Gray)
      mint::Color c =
          (h->grade() == 'S')
              ? mint::Color::kBrightCyan
              : (h->grade() == 'A' ? mint::Color::kYellow
                                   : (h->grade() == 'C' ? mint::Color::kGray
                                                        : mint::Color::kWhite));

      // 5명씩 2줄 배치 로직
      int row = i / 5;
      int col = i % 5;
      float rx = 20.0f + (col * 24.0f);
      float ry = 9.0f + (row * 8.0f);

      // 카드 레이아웃 렌더링
      renderer.Submit(L"┌─────────┐", {rx, ry}, c);
      renderer.Submit(L"│   [" +
                          std::wstring(1, static_cast<wchar_t>(h->grade())) +
                          L"]   │",
                      {rx, ry + 1}, c);

      // 이름 텍스트 오버플로우 방지 (7자 제한)
      std::wstring display_name = h->name();
      if (display_name.size() > 7) {
        display_name = display_name.substr(0, 6) + L"..";
      }
      ui_layout_->DrawTextAligned(static_cast<int>(rx) + 1,
                                  static_cast<int>(ry) + 2, 9, display_name,
                                  mint::ui::Alignment::Center, c);

      renderer.Submit(L"│ " + JobToWString(h->job()).substr(0, 4) + L" │",
                      {rx, ry + 3}, mint::Color::kGray);
      renderer.Submit(L"└─────────┘", {rx, ry + 4}, c);
    }

    // 모든 카드가 공개(reveal)된 후 안내 메시지
    if (!is_animating_) {
      ui_layout_->DrawTextAligned(
          7, 28, 146, L"[ Press any key to continue... ]",
          mint::ui::Alignment::Center, mint::Color::kBrightWhite);
    }
  }

  // 4. 하단 조작 가이드 (고정 UI)
  ui_layout_->DrawBox(7, 31, 146, 5, L" COMMAND ", mint::Color::kWhite);
  ui_layout_->DrawTextAligned(
      7, 33, 146,
      L"[G]: 1회 뽑기(100M)   [1]: 10회 뽑기(900G)   [ESC/TAB]: 뒤로 가기",
      mint::ui::Alignment::Center, mint::Color::kWhite);
}

}  // namespace guild