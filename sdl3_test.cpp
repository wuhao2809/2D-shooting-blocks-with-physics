#include <SDL3/SDL.h>
#include <iostream>

int main() {
  if (SDL_Init(0) != 0) {
    std::cerr << "SDL_Init failed: '" << SDL_GetError() << "'" << std::endl;
    return 1;
  }
  SDL_Window *win = SDL_CreateWindow("Test", 640, 480, 0);
  if (!win) {
    std::cerr << "SDL_CreateWindow failed: '" << SDL_GetError() << "'"
              << std::endl;
    SDL_Quit();
    return 1;
  }
  SDL_Delay(1000);
  SDL_DestroyWindow(win);
  SDL_Quit();
  std::cout << "SDL3 test succeeded!" << std::endl;
  return 0;
}