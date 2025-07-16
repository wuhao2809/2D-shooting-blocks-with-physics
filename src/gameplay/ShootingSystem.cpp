#include "ShootingSystem.hpp"
#include "../core/Manager.hpp"
#include "../core/Components.hpp"
#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm>

ShootingSystem::ShootingSystem(Manager *mgr) : manager(mgr)
{
    std::cout << "[ShootingSystem] Initialized" << std::endl;
}

void ShootingSystem::update(float dt)
{
    // Check for shoot requests from blackboard
    if (blackboard && blackboard->has("shoot_request") && blackboard->getValue<bool>("shoot_request"))
    {
        Entity shooterEntity = blackboard->getValue<Entity>("shoot_entity");
        float shootTime = blackboard->getValue<float>("shoot_time");

        handleShoot(shooterEntity, shootTime);

        // Clear the shoot request after handling
        blackboard->setValue("shoot_request", false);
    }

    updateBullets(dt);
    removeBulletsOutOfBounds();
    removeExpiredBullets();
}

void ShootingSystem::handleShoot(Entity shooterEntity, float currentTime)
{
    std::cout << "[ShootingSystem] Handling shoot for entity " << shooterEntity << std::endl;

    Position *pos = getComponent<Position>(shooterEntity);
    Direction *dir = getComponent<Direction>(shooterEntity);
    Shooter *shooter = getComponent<Shooter>(shooterEntity);

    if (!pos || !dir || !shooter || !shooter->canShoot)
    {
        std::cout << "[ShootingSystem] Cannot shoot - missing components or can't shoot" << std::endl;
        return;
    }

    // Check fire rate
    if (currentTime - shooter->lastShotTime < (1.0f / shooter->fireRate))
    {
        std::cout << "[ShootingSystem] Fire rate limit not met" << std::endl;
        return;
    }

    // Create bullet
    Entity bullet = createBullet(*pos, *dir);
    std::cout << "[ShootingSystem] Created bullet entity " << bullet << std::endl;

    shooter->lastShotTime = currentTime;
}

void ShootingSystem::updateBullets(float dt)
{
    // Update all bullets
    for (Entity entity : entities)
    {
        Bullet *bullet = getComponent<Bullet>(entity);
        if (!bullet)
            continue;

        Position *pos = getComponent<Position>(entity);
        Velocity *vel = getComponent<Velocity>(entity);

        if (!pos || !vel)
            continue;

        // Move bullet
        pos->x += vel->x * dt;
        pos->y += vel->y * dt;

        // Update lifetime
        bullet->timeAlive += dt;

        std::cout << "[ShootingSystem] Bullet " << entity << " at (" << pos->x << ", " << pos->y << ")" << std::endl;
    }
}

void ShootingSystem::removeBulletsOutOfBounds()
{
    bulletsToRemove.clear();

    for (Entity entity : entities)
    {
        Bullet *bullet = getComponent<Bullet>(entity);
        if (!bullet)
            continue;

        Position *pos = getComponent<Position>(entity);
        if (!pos)
            continue;

        // Check bounds (800x600 window)
        if (pos->x < -10 || pos->x > 810 || pos->y < -10 || pos->y > 610)
        {
            bulletsToRemove.push_back(entity);
            std::cout << "[ShootingSystem] Marking bullet " << entity << " for removal (out of bounds)" << std::endl;
        }
    }

    // Remove bullets
    for (Entity entity : bulletsToRemove)
    {
        manager->removeEntity(entity);
        // Remove from this system's entity list
        entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
        std::cout << "[ShootingSystem] Removed bullet " << entity << std::endl;
    }
}

void ShootingSystem::removeExpiredBullets()
{
    bulletsToRemove.clear();

    for (Entity entity : entities)
    {
        Bullet *bullet = getComponent<Bullet>(entity);
        if (!bullet)
            continue;

        if (bullet->timeAlive >= bullet->lifetime)
        {
            bulletsToRemove.push_back(entity);
            std::cout << "[ShootingSystem] Marking bullet " << entity << " for removal (expired)" << std::endl;
        }
    }

    // Remove bullets
    for (Entity entity : bulletsToRemove)
    {
        manager->removeEntity(entity);
        // Remove from this system's entity list
        entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
        std::cout << "[ShootingSystem] Removed bullet " << entity << std::endl;
    }
}

Entity ShootingSystem::createBullet(const Position &startPos, const Direction &dir)
{
    Entity bullet = manager->createEntity();

    // Position at shooter's center
    Position bulletPos = {startPos.x + 16, startPos.y + 16}; // Center of 32x32 player
    addComponent<Position>(bullet, bulletPos);

    // Calculate velocity based on direction
    float radians = dir.angle * M_PI / 180.0f;
    Velocity vel = {
        cos(radians) * 400.0f, // 400 pixels/second
        sin(radians) * 400.0f};
    addComponent<Velocity>(bullet, vel);

    // Bullet component
    Bullet bulletComp = {400.0f, 3.0f, 0.0f}; // speed, lifetime, timeAlive
    addComponent<Bullet>(bullet, bulletComp);

    // Make it renderable
    Renderable renderable = {"yellow", 4, 4, false}; // Small yellow square
    addComponent<Renderable>(bullet, renderable);

    // Add bullet to this system's entity list so it gets updated
    entities.push_back(bullet);

    std::cout << "[ShootingSystem] Created bullet with velocity (" << vel.x << ", " << vel.y << ")" << std::endl;

    return bullet;
}
