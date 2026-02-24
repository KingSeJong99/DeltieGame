#include <iostream>

#include "Level/GuildLevel.h"
#include "MintEngine/Engine/Engine.h"
#include "MintEngine/Level/Level.h"
#include "MintEngine/Render/Renderer.h"

int main() {

  mint::Engine engine;

  guild::GuildLevel * testLevel = new guild::GuildLevel();
  engine.SetNewLevel(testLevel);

  engine.Run();

  return 0;
}
