#pragma once
/**
 * @brief Base struct for ECS components. Extend for specific component types.
 */
struct Component {
  virtual ~Component() = default;
};