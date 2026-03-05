#include "MintEngine/Math/IntVector2.h"

#include <string>

namespace mint {

// 정적 상수 정의 (콘솔 좌표계 기준: Y-Down)
const IntVector2 IntVector2::kZero(0, 0);
const IntVector2 IntVector2::kOne(1, 1);
const IntVector2 IntVector2::kUp(0, -1);    // 위로 가면 Y 감소
const IntVector2 IntVector2::kDown(0, 1);    // 아래로 가면 Y 증가
const IntVector2 IntVector2::kLeft(-1, 0);
const IntVector2 IntVector2::kRight(1, 0);

std::string IntVector2::ToString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

// 참고: 생성자와 연산자 오버로딩은 헤더에 인라인으로 구현되어 있으므로 
// .cpp에서 중복 정의하지 않음.

}  // namespace mint
