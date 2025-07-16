#include "PhysicsSystem.hpp"
#include "../core/Manager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cmath>

PhysicsSystem::PhysicsSystem(Manager *mgr) : manager(mgr)
{
    // Create Box2D world with default settings
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f}; // No gravity for top-down shooter
    worldId = b2CreateWorld(&worldDef);

    std::cout << "[PhysicsSystem] Initialized with Box2D 3.x" << std::endl;
}

PhysicsSystem::~PhysicsSystem()
{
    if (b2World_IsValid(worldId))
    {
        b2DestroyWorld(worldId);
    }
}

void PhysicsSystem::update(float dt)
{
    if (!b2World_IsValid(worldId))
        return;

    // Check for new entity requests from blackboard
    if (blackboard && blackboard->has("physics_new_entity_request") &&
        blackboard->getValue<bool>("physics_new_entity_request"))
    {
        Entity entity = blackboard->getValue<Entity>("physics_new_entity");
        addEntity(entity);
        blackboard->setValue("physics_new_entity_request", false);
    }

    // Sync ECS data to physics world
    syncECSToPhysics();

    // Step the physics world
    b2World_Step(worldId, dt, 4);

    // Sync physics back to ECS
    syncPhysicsToECS();

    // Handle collisions
    handleCollisions();

    // Handle boundary collisions for obstacles
    handleBoundaryCollisions();
}

void PhysicsSystem::addEntity(Entity entity)
{
    entities.push_back(entity);

    // Check what type of entity this is and create appropriate body
    if (getComponent<Input>(entity))
    {
        createPlayerBody(entity);
        std::cout << "[PhysicsSystem] Added player entity " << entity << std::endl;
    }
    else if (getComponent<Bullet>(entity))
    {
        createBulletBody(entity);
        std::cout << "[PhysicsSystem] Added bullet entity " << entity << std::endl;
    }
    else if (getComponent<Position>(entity) && getComponent<Velocity>(entity))
    {
        createObstacleBody(entity);
        std::cout << "[PhysicsSystem] Added obstacle entity " << entity << std::endl;
    }
}

void PhysicsSystem::removeEntity(Entity entity)
{
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end())
    {
        entities.erase(it);
    }

    // Remove physics body if it exists
    auto bodyIt = entityBodies.find(entity);
    if (bodyIt != entityBodies.end())
    {
        b2DestroyBody(bodyIt->second);
        entityBodies.erase(bodyIt);
        std::cout << "[PhysicsSystem] Removed physics body for entity " << entity << std::endl;
    }
}

void PhysicsSystem::createPlayerBody(Entity entity)
{
    Position *pos = getComponent<Position>(entity);
    if (!pos)
        return;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pixelsToMeters(pos->x, pos->y);
    // Note: Box2D 3.x doesn't have fixedRotation in bodyDef, we'll handle it differently

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    // Create shape for player
    b2Polygon box = b2MakeBox(16.0f * METERS_PER_PIXEL, 16.0f * METERS_PER_PIXEL);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    // Note: Box2D 3.x doesn't have friction in shapeDef, it's handled differently

    b2CreatePolygonShape(bodyId, &shapeDef, &box);

    entityBodies[entity] = bodyId;

    std::cout << "[PhysicsSystem] Created player body for entity " << entity << std::endl;
}

void PhysicsSystem::createBulletBody(Entity entity)
{
    Position *pos = getComponent<Position>(entity);
    if (!pos)
        return;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pixelsToMeters(pos->x, pos->y);
    bodyDef.isBullet = true; // Enable continuous collision detection

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    // Create small circle shape for bullet
    b2Circle circle = {{0, 0}, 2.0f * METERS_PER_PIXEL};
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.1f;
    shapeDef.isSensor = true; // Bullets are sensors for collision detection

    b2CreateCircleShape(bodyId, &shapeDef, &circle);

    entityBodies[entity] = bodyId;

    std::cout << "[PhysicsSystem] Created bullet body for entity " << entity << std::endl;
}

void PhysicsSystem::createObstacleBody(Entity entity)
{
    Position *pos = getComponent<Position>(entity);
    Renderable *renderable = getComponent<Renderable>(entity);
    if (!pos || !renderable)
        return;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pixelsToMeters(pos->x, pos->y);

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    // Create box shape for obstacle
    b2Polygon box = b2MakeBox(
        renderable->width * 0.5f * METERS_PER_PIXEL,
        renderable->height * 0.5f * METERS_PER_PIXEL);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 2.0f;

    b2CreatePolygonShape(bodyId, &shapeDef, &box);

    entityBodies[entity] = bodyId;

    std::cout << "[PhysicsSystem] Created obstacle body for entity " << entity << std::endl;
}

void PhysicsSystem::syncECSToPhysics()
{
    for (Entity entity : entities)
    {
        auto bodyIt = entityBodies.find(entity);
        if (bodyIt == entityBodies.end())
            continue;

        b2BodyId bodyId = bodyIt->second;
        Position *pos = getComponent<Position>(entity);
        Velocity *vel = getComponent<Velocity>(entity);

        if (pos)
        {
            b2Vec2 position = pixelsToMeters(pos->x, pos->y);
            b2Rot rotation = b2MakeRot(1.0f); // No rotation for top-down
            b2Body_SetTransform(bodyId, position, rotation);
        }

        if (vel)
        {
            b2Vec2 velocity = {vel->x * METERS_PER_PIXEL, vel->y * METERS_PER_PIXEL};
            b2Body_SetLinearVelocity(bodyId, velocity);
        }
    }
}

void PhysicsSystem::syncPhysicsToECS()
{
    for (Entity entity : entities)
    {
        auto bodyIt = entityBodies.find(entity);
        if (bodyIt == entityBodies.end())
            continue;

        b2BodyId bodyId = bodyIt->second;
        Position *pos = getComponent<Position>(entity);
        Velocity *vel = getComponent<Velocity>(entity);

        if (pos)
        {
            b2Vec2 position = b2Body_GetPosition(bodyId);
            metersToPixels(position, pos->x, pos->y);
        }

        if (vel)
        {
            b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);
            vel->x = velocity.x * PIXELS_PER_METER;
            vel->y = velocity.y * PIXELS_PER_METER;
        }
    }
}

void PhysicsSystem::handleCollisions()
{
    // Simple collision detection for bullet-obstacle collisions
    for (Entity bullet : entities)
    {
        if (!getComponent<Bullet>(bullet))
            continue;

        Position *bulletPos = getComponent<Position>(bullet);
        if (!bulletPos)
            continue;

        for (Entity obstacle : entities)
        {
            if (bullet == obstacle ||
                getComponent<Bullet>(obstacle) ||
                getComponent<Input>(obstacle))
                continue;

            Position *obstaclePos = getComponent<Position>(obstacle);
            Renderable *obstacleRend = getComponent<Renderable>(obstacle);

            if (!obstaclePos || !obstacleRend)
                continue;

            // Simple bounding box collision check
            float dx = bulletPos->x - obstaclePos->x;
            float dy = bulletPos->y - obstaclePos->y;

            if (abs(dx) < obstacleRend->width / 2 && abs(dy) < obstacleRend->height / 2)
            {
                handleBulletObstacleCollision(bullet, obstacle);
            }
        }
    }

    // Check for player-obstacle collisions
    for (Entity player : entities)
    {
        if (!getComponent<Input>(player))
            continue;

        Position *playerPos = getComponent<Position>(player);
        if (!playerPos)
            continue;

        for (Entity obstacle : entities)
        {
            if (player == obstacle ||
                getComponent<Bullet>(obstacle) ||
                getComponent<Input>(obstacle))
                continue;

            Position *obstaclePos = getComponent<Position>(obstacle);
            Renderable *obstacleRend = getComponent<Renderable>(obstacle);

            if (!obstaclePos || !obstacleRend)
                continue;

            // Simple bounding box collision check (player is 32x32, obstacle varies)
            float dx = playerPos->x - obstaclePos->x;
            float dy = playerPos->y - obstaclePos->y;
            float playerHalfSize = 16.0f; // Player is 32x32, so half is 16

            if (abs(dx) < (playerHalfSize + obstacleRend->width / 2) &&
                abs(dy) < (playerHalfSize + obstacleRend->height / 2))
            {
                handlePlayerObstacleCollision(player, obstacle);
            }
        }
    }
}

void PhysicsSystem::handleBulletObstacleCollision(Entity bullet, Entity obstacle)
{
    // Apply impulse to obstacle
    Velocity *obstacleVel = getComponent<Velocity>(obstacle);
    if (obstacleVel)
    {
        // Apply impulse based on bullet direction
        Velocity *bulletVel = getComponent<Velocity>(bullet);
        if (bulletVel)
        {
            float impulseStrength = 100.0f;
            float bulletSpeed = sqrt(bulletVel->x * bulletVel->x + bulletVel->y * bulletVel->y);
            if (bulletSpeed > 0)
            {
                obstacleVel->x += (bulletVel->x / bulletSpeed) * impulseStrength;
                obstacleVel->y += (bulletVel->y / bulletSpeed) * impulseStrength;
            }
        }
    }

    // Remove bullet immediately upon collision
    if (manager)
    {
        manager->removeEntity(bullet);
        // Remove from physics system's entity list
        entities.erase(std::remove(entities.begin(), entities.end(), bullet), entities.end());
        // Remove from physics bodies map
        auto bodyIt = entityBodies.find(bullet);
        if (bodyIt != entityBodies.end())
        {
            b2DestroyBody(bodyIt->second);
            entityBodies.erase(bodyIt);
        }
    }

    // Post collision event to blackboard for other systems (like MapSystem)
    if (blackboard)
    {
        blackboard->setValue("collision_event", true);
        blackboard->setValue("collision_bullet", bullet);
        blackboard->setValue("collision_obstacle", obstacle);
    }

    std::cout << "[PhysicsSystem] Bullet " << bullet << " hit obstacle " << obstacle << " and was removed" << std::endl;
}

void PhysicsSystem::handlePlayerObstacleCollision(Entity player, Entity obstacle)
{
    Position *playerPos = getComponent<Position>(player);
    Position *obstaclePos = getComponent<Position>(obstacle);
    Velocity *playerVel = getComponent<Velocity>(player);
    Velocity *obstacleVel = getComponent<Velocity>(obstacle);

    if (!playerPos || !obstaclePos || !playerVel || !obstacleVel)
        return;

    // Calculate collision direction (from obstacle to player)
    float dx = playerPos->x - obstaclePos->x;
    float dy = playerPos->y - obstaclePos->y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > 0)
    {
        // Normalize direction
        dx /= distance;
        dy /= distance;

        // Apply impulse to player (knockback effect)
        float knockbackStrength = 150.0f;
        playerVel->x += dx * knockbackStrength;
        playerVel->y += dy * knockbackStrength;

        // Apply smaller counter-impulse to obstacle
        float obstacleImpulse = 50.0f;
        obstacleVel->x -= dx * obstacleImpulse;
        obstacleVel->y -= dy * obstacleImpulse;

        // Post player collision event to blackboard
        if (blackboard)
        {
            blackboard->setValue("player_collision_event", true);
            blackboard->setValue("player_collision_player", player);
            blackboard->setValue("player_collision_obstacle", obstacle);
        }

        std::cout << "[PhysicsSystem] Player " << player << " collided with obstacle " << obstacle << std::endl;
    }
}

void PhysicsSystem::handleBoundaryCollisions()
{
    // Check all obstacles for boundary collisions
    for (Entity entity : entities)
    {
        // Skip bullets and player
        if (getComponent<Bullet>(entity) || getComponent<Input>(entity))
            continue;

        // Only check obstacles (entities with Position, Renderable, and Velocity)
        Position *pos = getComponent<Position>(entity);
        Renderable *renderable = getComponent<Renderable>(entity);
        Velocity *vel = getComponent<Velocity>(entity);

        if (!pos || !renderable || !vel)
            continue;

        handleObstacleBoundaryCollision(entity);
    }
}

void PhysicsSystem::handleObstacleBoundaryCollision(Entity obstacle)
{
    Position *pos = getComponent<Position>(obstacle);
    Renderable *renderable = getComponent<Renderable>(obstacle);
    Velocity *vel = getComponent<Velocity>(obstacle);

    if (!pos || !renderable || !vel)
        return;

    // Calculate boundary positions considering obstacle size
    float halfWidth = renderable->width / 2.0f;
    float halfHeight = renderable->height / 2.0f;

    float leftBoundary = halfWidth;
    float rightBoundary = 800 - halfWidth; // WINDOW_WIDTH = 800
    float topBoundary = halfHeight;
    float bottomBoundary = 600 - halfHeight; // WINDOW_HEIGHT = 600

    bool bounced = false;

    // Check left and right boundaries
    if (pos->x <= leftBoundary && vel->x < 0)
    {
        vel->x = -vel->x;      // Reverse horizontal velocity
        pos->x = leftBoundary; // Keep obstacle within bounds
        bounced = true;
    }
    else if (pos->x >= rightBoundary && vel->x > 0)
    {
        vel->x = -vel->x;       // Reverse horizontal velocity
        pos->x = rightBoundary; // Keep obstacle within bounds
        bounced = true;
    }

    // Check top and bottom boundaries
    if (pos->y <= topBoundary && vel->y < 0)
    {
        vel->y = -vel->y;     // Reverse vertical velocity
        pos->y = topBoundary; // Keep obstacle within bounds
        bounced = true;
    }
    else if (pos->y >= bottomBoundary && vel->y > 0)
    {
        vel->y = -vel->y;        // Reverse vertical velocity
        pos->y = bottomBoundary; // Keep obstacle within bounds
        bounced = true;
    }

    // Apply damping to prevent infinite bouncing
    if (bounced)
    {
        float dampingFactor = 0.8f; // Reduce velocity by 20% on bounce
        vel->x *= dampingFactor;
        vel->y *= dampingFactor;

        // Post boundary collision event to blackboard
        if (blackboard)
        {
            blackboard->setValue("boundary_collision_event", true);
            blackboard->setValue("boundary_collision_entity", obstacle);
        }

        std::cout << "[PhysicsSystem] Obstacle " << obstacle << " bounced off boundary" << std::endl;
    }
}

void PhysicsSystem::processCollisionEvents()
{
    // Stub implementation
}

b2Vec2 PhysicsSystem::pixelsToMeters(float pixelX, float pixelY)
{
    return {pixelX * METERS_PER_PIXEL, pixelY * METERS_PER_PIXEL};
}

void PhysicsSystem::metersToPixels(const b2Vec2 &meters, float &pixelX, float &pixelY)
{
    pixelX = meters.x * PIXELS_PER_METER;
    pixelY = meters.y * PIXELS_PER_METER;
}

// Box2D 3.x collision callbacks
bool beginContactCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold *manifold, void *context)
{
    return true;
}

bool endContactCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void *context)
{
    return true;
}