#include <iostream>

#include "MintEngine/Engine/Engine.h"
#include "MintEngine/Level/Level.h"
#include "MintEngine/Render/Renderer.h"

using namespace mint;

class TestLevel : public Level {
 public:
  void Draw(Renderer& renderer, int width, int height) override {
    // 1. 박스 그리기 (전각 기호 테스트)
    // 상단
    renderer.SetCell(10, 5, L'┏', Color::kCyan, Color::kBlack);
    for (int i = 11; i < 30; ++i)
      renderer.SetCell(i, 5, L'━', Color::kCyan, Color::kBlack);
    renderer.SetCell(30, 5, L'┓', Color::kCyan, Color::kBlack);

    // 측면
    for (int i = 6; i < 15; ++i) {
      renderer.SetCell(10, i, L'┃', Color::kCyan, Color::kBlack);
      renderer.SetCell(30, i, L'┃', Color::kCyan, Color::kBlack);
    }

    // 하단
    renderer.SetCell(10, 15, L'┗', Color::kCyan, Color::kBlack);
    for (int i = 11; i < 30; ++i)
      renderer.SetCell(i, 15, L'━', Color::kCyan, Color::kBlack);
    renderer.SetCell(30, 15, L'┛', Color::kCyan, Color::kBlack);

    // 2. 한글 출력 테스트 (전각 문자 테스트)
    renderer.Submit(L"시엘 엔진 유니코드 테스트", Vector2(12, 7),
                    Color::kYellow);
    renderer.Submit(L"바보야, 잘 보여?", Vector2(15, 10), Color::kWhite);

    // 3. 다양한 색상의 점 찍기 (SetCell 테스트)
    renderer.SetCell(20, 12, L'★', Color::kRed, Color::kBlack);
    renderer.SetCell(22, 12, L'♥', Color::kMagenta, Color::kBlack);

    // 부모 클래스의 Draw 호출 (액터들 그리기)
    Level::Draw(renderer, width, height);
  }
};

int main() {

  Engine engine;

  TestLevel* testLevel = new TestLevel();
  engine.SetNewLevel(testLevel);

  engine.Run();

  return 0;
}
