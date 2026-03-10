#ifndef MINT_ENGINE_CORE_CAMERA_H_
#define MINT_ENGINE_CORE_CAMERA_H_

#include <algorithm>

namespace mint {

    /**
     * @enum ViewMode
     * @brief 현재 렌더링할 레벨을 어떻게 출력할 것인지 알려주는 열거형.
     */
    enum class ViewMode {
        kGlobal,        ///< 전체 맵을 1:1로 출력.
        kFocus          ///< 특정 대상 중심으로 3by6 격자로 출력 (확대해서 보기).
    };

/**
 * @class Camera
 * @brief 월드 좌표와 화면 좌표 사이의 오프셋을 관리하는 클래스.
 */
class Camera {
 public:
  Camera(int view_w, int view_h) : view_w_(view_w), view_h_(view_h) {}

  /** @biref 바라볼 목표 좌표(월드)를 설정하고 경계값에 맞게 클램핑한다. */
  void LookAt(int world_x, int world_y, int max_w, int max_h) {
    offset_x_ = (std::max)(0, (std::min)(world_x - view_w_ / 2, max_w - view_w_));
    offset_y_ = (std::max)(0, (std::min)(world_y - view_h_ / 2, max_h - view_h_));
  }

  /** @brief 월드 좌표를 화면(상대) 좌표로 변환한다.*/
  inline int WorldToScreenX(int world_x) const { return world_x - offset_x_; }
  inline int WorldToScreenY(int world_y) const { return world_y - offset_y_; }

  // --- Getter ---
  int offset_x() const { return offset_x_; }
  int offset_y() const { return offset_y_; }
  int view_w() const { return view_w_; }
  int view_h() const { return view_h_; }

 private:
  int offset_x_ = 0;	///< 월드 기준 카메라 시작 X
  int offset_y_ = 0;	///< 월드 기준 카메라 시작 Y
  int view_w_ = 0;		///< 화면에 보여줄 가로 타일 개수
  int view_h_ = 0;		///< 화면에 보여줄 세로 타일 개수
};
}  // namespace mint

#endif / MINT_ENGINE_CORE_CAMERA_H_