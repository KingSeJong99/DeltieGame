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
 * @brief 게임 내 모든 객체의 기본 클래스
 */
class MINT_API Actor : public RTTI {
  RTTI_DECLARATIONS(Actor, RTTI)

 public:
  /**
   * @brief Actor 생성자
   * @param image 화면에 표시될 유니코드 문자열
   * @param position 초기 좌표
   * @param color 초기 색상
   */
  explicit Actor(const std::wstring& image = L" ", 
                 const Vector2& position = Vector2::kZero,
                 Color color = Color::kWhite);
  virtual ~Actor();

  // 게임 플레이 이벤트
  virtual void BeginPlay();
  virtual void Tick(float delta_time);
  virtual void Draw(CHAR_INFO* back_buffer, int width, int height);

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

  std::wstring image_;     ///< 화면에 표시될 유니코드 문자열
  Color color_ = Color::kWhite;
  Level* owner_ = nullptr;
  int sorting_order_ = 0;

 private:
  Vector2 position_;
};

}  // namespace mint

#endif  // MINT_ENGINE_ACTOR_ACTOR_H_
