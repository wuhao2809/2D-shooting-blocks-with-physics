#pragma once
#include "Component.hpp"
#include "Entity.hpp"
#include "System.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>

/**
 * @brief ECS Manager: Handles entities, components, and systems.
 */
class Manager {
public:
  Manager();
  ~Manager();
  Entity createEntity();
  void destroyEntity(Entity entity);
  // Component and system management methods to be implemented
private:
  Entity nextEntityId = 1;
  // Storage for components and systems
};