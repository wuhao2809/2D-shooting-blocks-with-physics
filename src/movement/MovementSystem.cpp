#include "MovementSystem.hpp"
#include "../core/Manager.hpp"
#include <iostream>

void MovementSystem::update(float dt)
{
    // Process movement requests from blackboard
    if (blackboard && blackboard->has("movement_request") && blackboard->getValue<bool>("movement_request"))
    {
        Entity entity = blackboard->getValue<Entity>("movement_entity");
        float moveX = blackboard->getValue<float>("movement_x");
        float moveY = blackboard->getValue<float>("movement_y");

        // Apply movement to entity's velocity
        Velocity *vel = getComponent<Velocity>(entity);
        if (vel)
        {
            vel->x = moveX;
            vel->y = moveY;
        }

        // Clear movement request
        blackboard->setValue("movement_request", false);
    }

    // Update all entities with Position and Velocity components
    for (Entity entity : entities)
    {
        applyVelocityEffects(entity, dt);
        applyBoundaryConstraints(entity);
    }
}

void MovementSystem::applyVelocityEffects(Entity entity, float dt)
{
    Position *pos = getComponent<Position>(entity);
    Velocity *vel = getComponent<Velocity>(entity);

    if (!pos || !vel)
        return;

    // Position updates are now handled by PhysicsSystem only
    // This system only handles velocity modifications (friction/damping)

    // Apply friction/damping for non-bullet entities
    Bullet *bullet = getComponent<Bullet>(entity);
    if (!bullet)
    {
        // Apply friction to reduce velocity over time
        float friction = 0.95f;
        vel->x *= friction;
        vel->y *= friction;

        // Stop very small velocities to prevent jittering
        if (abs(vel->x) < 0.1f)
            vel->x = 0.0f;
        if (abs(vel->y) < 0.1f)
            vel->y = 0.0f;
    }
}

void MovementSystem::applyBoundaryConstraints(Entity entity)
{
    Position *pos = getComponent<Position>(entity);
    Velocity *vel = getComponent<Velocity>(entity);
    if (!pos || !vel)
        return;

    // Check if entity is a player for velocity-based boundary constraints
    Input *input = getComponent<Input>(entity);
    Bullet *bullet = getComponent<Bullet>(entity);

    if (input && input->controllable)
    {
        // Player boundary constraints - prevent velocity that would move player out of bounds
        // Position constraints are now handled by PhysicsSystem
        if (pos->x <= 0 && vel->x < 0)
            vel->x = 0; // Stop leftward movement at left edge
        if (pos->y <= 0 && vel->y < 0)
            vel->y = 0; // Stop upward movement at top edge
        if (pos->x >= 800 - 32 && vel->x > 0)
            vel->x = 0; // Stop rightward movement at right edge (32 is player width)
        if (pos->y >= 600 - 32 && vel->y > 0)
            vel->y = 0; // Stop downward movement at bottom edge (32 is player height)
    }
    else if (bullet)
    {
        // Bullets are handled by ShootingSystem for out-of-bounds removal
        // No boundary constraints here
    }
}
