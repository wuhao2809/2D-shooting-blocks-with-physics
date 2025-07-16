#pragma once
#include "../core/System.hpp"
#include "../core/Components.hpp"

/**
 * @brief System to handle movement of all entities with Position and Velocity components
 */
class MovementSystem : public System
{
public:
    MovementSystem() = default;
    void update(float dt) override;

private:
    void updateEntityPosition(Entity entity, float dt);
    void applyBoundaryConstraints(Entity entity);
};
