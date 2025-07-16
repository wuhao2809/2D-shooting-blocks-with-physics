#include "InputSystem.hpp"
#include "../core/Manager.hpp"
#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>

void InputSystem::update(float dt)
{
  static float gameTime = 0.0f;
  gameTime += dt;

  // For each entity with Input and Position
  for (Entity entity : entities)
  {
    Input *input = getComponent<Input>(entity);
    Position *pos = getComponent<Position>(entity);
    if (!input || !pos || !input->controllable)
      continue;

    float speed = 200.0f; // pixels per second
    bool moved = false;
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (keyStates[entity][SDL_SCANCODE_W])
    {
      moveY = -speed;
      moved = true;
    }
    if (keyStates[entity][SDL_SCANCODE_S])
    {
      moveY = speed;
      moved = true;
    }
    if (keyStates[entity][SDL_SCANCODE_A])
    {
      moveX = -speed;
      moved = true;
    }
    if (keyStates[entity][SDL_SCANCODE_D])
    {
      moveX = speed;
      moved = true;
    }

    // Post movement request to blackboard
    if (moved && blackboard)
    {
      blackboard->setValue("movement_request", true);
      blackboard->setValue("movement_entity", entity);
      blackboard->setValue("movement_x", moveX);
      blackboard->setValue("movement_y", moveY);
    }

    // Update player direction based on movement
    if (moved)
    {
      updatePlayerDirection(entity);
    }

    // Handle shooting
    if (keyStates[entity][SDL_SCANCODE_SPACE])
    {
      handleShooting(entity, gameTime);
    }

    std::cout << "[InputSystem] Player at (" << pos->x << ", " << pos->y << ")" << std::endl;
  }
}

void InputSystem::updatePlayerDirection(Entity entity)
{
  Direction *dir = getComponent<Direction>(entity);
  if (!dir)
    return;

  bool w = keyStates[entity][SDL_SCANCODE_W];
  bool s = keyStates[entity][SDL_SCANCODE_S];
  bool a = keyStates[entity][SDL_SCANCODE_A];
  bool d = keyStates[entity][SDL_SCANCODE_D];

  // Calculate direction based on movement keys
  float newAngle = dir->angle; // Keep current angle if no movement

  if (d && !a)
  { // Right
    if (w && !s)
      newAngle = 315.0f; // Up-right
    else if (s && !w)
      newAngle = 45.0f; // Down-right
    else
      newAngle = 0.0f; // Right
  }
  else if (a && !d)
  { // Left
    if (w && !s)
      newAngle = 225.0f; // Up-left
    else if (s && !w)
      newAngle = 135.0f; // Down-left
    else
      newAngle = 180.0f; // Left
  }
  else if (w && !s)
  {                    // Up
    newAngle = 270.0f; // Up
  }
  else if (s && !w)
  {                   // Down
    newAngle = 90.0f; // Down
  }

  dir->angle = newAngle;
  std::cout << "[InputSystem] Player direction updated to " << dir->angle << " degrees" << std::endl;
}

void InputSystem::handleShooting(Entity entity, float currentTime)
{
  if (blackboard)
  {
    // Post shooting request to blackboard
    blackboard->setValue("shoot_request", true);
    blackboard->setValue("shoot_entity", entity);
    blackboard->setValue("shoot_time", currentTime);
    std::cout << "[InputSystem] Posted shoot request to blackboard" << std::endl;
  }
}

void InputSystem::handleEvent(const SDL_Event &event)
{
  if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
  {
    bool pressed = (event.type == SDL_EVENT_KEY_DOWN);
    SDL_Scancode sc = event.key.scancode; // SDL3: use scancode field

    // Handle special keys
    if (pressed && blackboard)
    {
      if (sc == SDL_SCANCODE_H)
      {
        // Toggle HUD
        blackboard->setValue("hud_toggle_request", true);
        std::cout << "[InputSystem] HUD toggle requested" << std::endl;
      }
      else if (sc == SDL_SCANCODE_ESCAPE)
      {
        // Exit game
        blackboard->setValue("exit_game_request", true);
        std::cout << "[InputSystem] Exit game requested" << std::endl;
      }
    }

    for (Entity entity : entities)
    {
      keyStates[entity][sc] = pressed;
    }

    std::cout << "[InputSystem] Key " << sc << " " << (pressed ? "pressed" : "released") << std::endl;
  }
}