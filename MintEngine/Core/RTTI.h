#ifndef MINT_ENGINE_CORE_RTTI_H_
#define MINT_ENGINE_CORE_RTTI_H_

#include <cstddef>

#include "MintEngine/Core/Common.h"

namespace mint {

/**
 * @brief 런타임 타입 정보를 제공하기 위한 추상 베이스 클래스
 */
class MINT_API RTTI {
 public:
  virtual ~RTTI() = default;

  virtual const size_t& GetType() const = 0;

  virtual bool Is(const RTTI* const rtti) const { return false; }
  virtual bool Is(const size_t id) const { return false; }

  template <typename T>
  bool IsTypeOf() const {
    return Is(T::TypeIdClass());
  }

  template <typename T>
  T* As() {
    if (Is(T::TypeIdClass())) {
      return static_cast<T*>(this);
    }
    return nullptr;
  }

  template <typename T>
  const T* As() const {
    if (Is(T::TypeIdClass())) {
      return static_cast<const T*>(this);
    }
    return nullptr;
  }
};

}  // namespace mint

/**
 * @brief RTTI를 사용하는 클래스 선언부에 포함해야 하는 매크로
 */
#define RTTI_DECLARATIONS(Type, ParentType)                     \
 public:                                                        \
  static size_t TypeIdClass() {                                 \
    static int run_time_type_id = 0;                            \
    return reinterpret_cast<size_t>(&run_time_type_id);         \
  }                                                             \
                                                                \
  virtual const size_t& GetType() const override {              \
    return Type::TypeIdClass();                                 \
  }                                                             \
                                                                \
  using super = ParentType;                                     \
                                                                \
  virtual bool Is(const size_t id) const override {             \
    if (id == TypeIdClass()) {                                  \
      return true;                                              \
    }                                                           \
    return ParentType::Is(id);                                  \
  }                                                             \
                                                                \
  virtual bool Is(const mint::RTTI* const rtti) const override { \
    if (rtti == nullptr) return false;                          \
    return Is(rtti->GetType());                                 \
  }

#endif  // MINT_ENGINE_CORE_RTTI_H_
