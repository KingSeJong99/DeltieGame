#ifndef MINT_ENGINE_ACTOR_ACTOR_H_
#define MINT_ENGINE_ACTOR_ACTOR_H_

#include <Windows.h>

#include <string>

#include "MintEngine/Core/Color.h"
#include "MintEngine/Core/RTTI.h"
#include "MintEngine/Math/Vector2.h"

namespace mint {

class Level;

/**
 * @class Actor
 * @brief 게임 내 모든 객체의 기본 클래스.
 * @note 위치, 외형(문자), 색상 등의 기본 정보를 가지며, Level에 의해 관리된다.
 */
class MINT_API Actor : public RTTI {
  RTTI_DECLARATIONS(Actor, RTTI)

 public:
  /**
   * @brief Actor 생성자.
   * @param name 액터 식별을 위한 이름.
   * @param image 화면에 표시될 유니코드 문자열.
   * @param position 초기 좌표. (기본값 Vector2::kZero)
   * @param color 초기 색상. (기본값 Color::kWhite)
   */
  explicit Actor(const std::wstring& name = L"UnknownActor",
                 const std::wstring& image = L" ",
                 const Vector2& position = Vector2::kZero,
                 Color color = Color::kWhite);
  virtual ~Actor();

  // 게임 플레이 이벤트
  virtual void BeginPlay();
  virtual void Tick(float delta_time);
  virtual void Draw(CHAR_INFO* back_buffer, int width, int height);

  // 이름 설정 및 반환
  inline void set_name(const std::wstring& name) { name_ = name; }
  inline const std::wstring& name() const { return name_; }

  // 위치 설정 및 반환
  void set_position(const Vector2& new_position);
  inline Vector2 position() const { return position_; }

  // 소유자(Level) 설정 및 반환
  inline void set_owner(Level* new_owner) { owner_ = new_owner; }
  inline Level* owner() const { return owner_; }

  // 상태 확인용 Getter
  inline bool has_began_play() const { return has_began_play_; }
  inline bool is_active() const { return is_active_ && !destroy_requested_; }
  inline bool destroy_requested() const { return destroy_requested_; }
  inline int sorting_order() const { return sorting_order_; }

 protected:
  bool has_began_play_ = false;
  bool is_active_ = true;
  bool destroy_requested_ = false;

  std::wstring name_;   ///< 액터 식별용 이름. (로그 출력 시 사용)
  std::wstring image_;  ///< 화면에 표시될 유니코드 문자열
  Color color_ = Color::kWhite;
  Level* owner_ = nullptr;
  int sorting_order_ = 0;

 private:
  Vector2 position_;
};

}  // namespace mint

#endif  // MINT_ENGINE_ACTOR_ACTOR_H_
