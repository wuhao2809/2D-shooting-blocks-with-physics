#pragma once
#include "Entity.hpp"
#include <cstdint>
#include <vector>

/**
 * @brief Base class for ECS systems. Override update() in derived systems.
 */
class System {
public:
  virtual ~System() = default;
  virtual void update(float dt) = 0;
  std::vector<Entity> entities;
};