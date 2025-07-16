#pragma once
#include "../core/System.hpp"
#include "../core/Components.hpp"
#include <unordered_map>
#include <vector>

class Manager; // Forward declaration

class ShootingSystem : public System
{
public:
    ShootingSystem(Manager *manager);
    void update(float dt) override;
    void handleShoot(Entity shooterEntity, float currentTime);

private:
    Manager *manager;
    std::vector<Entity> bulletsToRemove;

    void updateBullets(float dt);
    void removeBulletsOutOfBounds();
    void removeExpiredBullets();
    Entity createBullet(const Position &startPos, const Direction &dir);
};
