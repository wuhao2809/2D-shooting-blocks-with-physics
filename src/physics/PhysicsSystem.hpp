#pragma once
#include "../core/System.hpp"
#include "../core/Components.hpp"
#include <box2d/box2d.h>
#include <unordered_map>

class Manager;

/**
 * @brief Physics system using Box2D 3.x for collision detection and physics simulation
 */
class PhysicsSystem : public System
{
public:
    PhysicsSystem(Manager *mgr);
    ~PhysicsSystem();

    void update(float dt) override;
    void addEntity(Entity entity);
    void removeEntity(Entity entity);

    // Physics world settings
    static constexpr float PIXELS_PER_METER = 32.0f;
    static constexpr float METERS_PER_PIXEL = 1.0f / PIXELS_PER_METER;

private:
    Manager *manager;
    b2WorldId worldId;
    std::unordered_map<Entity, b2BodyId> entityBodies;

    void createPlayerBody(Entity entity);
    void createBulletBody(Entity entity);
    void createObstacleBody(Entity entity);
    void syncPhysicsToECS();
    void syncECSToPhysics();
    void handleCollisions();
    void handleBoundaryCollisions();

    // Helper functions
    b2Vec2 pixelsToMeters(float pixelX, float pixelY);
    void metersToPixels(const b2Vec2 &meters, float &pixelX, float &pixelY);
    void handleBulletObstacleCollision(Entity bullet, Entity obstacle);
    void handlePlayerObstacleCollision(Entity player, Entity obstacle);
    void handleObstacleBoundaryCollision(Entity obstacle);

    // Box2D 3.x doesn't use contact listeners, use collision events instead
    void processCollisionEvents();
};

// Box2D 3.x collision callbacks
bool beginContactCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold *manifold, void *context);
bool endContactCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void *context);
