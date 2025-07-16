#include "PhysicsSystem.hpp"
#include "../core/Manager.hpp"
#include <iostream>
#include <algorithm>
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
    // Basic update stub
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
        
    // Step the physics world
    b2World_Step(worldId, dt, 4);
    
    // Handle collisions
    handleCollisions();
}

void PhysicsSystem::addEntity(Entity entity)
{
    entities.push_back(entity);
}

void PhysicsSystem::removeEntity(Entity entity)
{
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end())
    {
        entities.erase(it);
    }
}

void PhysicsSystem::createPlayerBody(Entity entity)
{
    // Stub implementation
}

void PhysicsSystem::createBulletBody(Entity entity)
{
    // Stub implementation
}

void PhysicsSystem::createObstacleBody(Entity entity)
{
    // Stub implementation
}

void PhysicsSystem::syncECSToPhysics()
{
    // Stub implementation
}

void PhysicsSystem::syncPhysicsToECS()
{
    // Stub implementation
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
    
    // Post collision event to blackboard
    if (blackboard)
    {
        blackboard->setValue("collision_event", true);
        blackboard->setValue("collision_bullet", bullet);
        blackboard->setValue("collision_obstacle", obstacle);
    }
    
    std::cout << "[PhysicsSystem] Bullet " << bullet << " hit obstacle " << obstacle << std::endl;
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