#include <iostream>

#include "MintEngine/Core/Logger.h"
#include "Level/GuildLevel.h"
#include "MintEngine/Engine/Engine.h"
#include "MintEngine/Level/Level.h"
#include "MintEngine/Render/Renderer.h"

int main() {
  MINT_LOG_INFO(
      L"로그를 초기화합니다.\n\n\n");
  mint::Engine engine;

  guild::GuildLevel * testLevel = new guild::GuildLevel();
  if (testLevel == nullptr) {
    MINT_LOG_ERROR_TAG(L"GuildLevel", L"생성에 실패했습니다!");
    return -1;
  }

  engine.SetNewLevel(testLevel);

  engine.Run();

  return 0;
}
