#include "core/GameEngine.hpp"
#include <iostream>

int main()
{
  GameEngine game;

  if (!game.initialize())
  {
    std::cerr << "Failed to initialize game engine" << std::endl;
    return 1;
  }

  game.run();

  return 0;
}
