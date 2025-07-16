#pragma once
#include "Entity.hpp"
#include "Blackboard.hpp"
#include <cstdint>
#include <vector>

/**
 * @brief Base class for ECS systems. Override update() in derived systems.
 */
class System
{
public:
  virtual ~System() = default;
  virtual void update(float dt) = 0;

  // Set blackboard reference for inter-system communication
  void setBlackboard(Blackboard *bb) { blackboard = bb; }

  std::vector<Entity> entities;

protected:
  Blackboard *blackboard = nullptr;
};