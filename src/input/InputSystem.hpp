#pragma once
#include "../core/Components.hpp"
#include "../core/System.hpp"
#include <SDL3/SDL.h>
#include <unordered_map>

/**
 * @brief System to handle player input (WSAD) and update positions.
 */
class InputSystem : public System
{
public:
  InputSystem() = default;
  void update(float dt) override;
  void handleEvent(const SDL_Event &event);

  // Map of entity to pressed keys
  std::unordered_map<Entity, std::unordered_map<SDL_Scancode, bool>> keyStates;

private:
  void updatePlayerDirection(Entity entity);
  void handleShooting(Entity entity, float currentTime);
};