#pragma once
#include "../core/System.hpp"
#include "../core/Components.hpp"
#include <box2d/box2d.h>
#include <unordered_map>
#include <memory>

/**
 * @brief Physics system using Box2D for collision detection and physics simulation
 */
class PhysicsSystem : public System
{
public:
    PhysicsSystem();
    ~PhysicsSystem();
    
    void update(float dt) override;
    void addEntity(Entity entity);
    void removeEntity(Entity entity);
    
    // Physics world settings
    static constexpr float PIXELS_PER_METER = 32.0f;
    static constexpr float METERS_PER_PIXEL = 1.0f / PIXELS_PER_METER;

private:
    std::unique_ptr<b2World> world;
    std::unordered_map<Entity, b2Body*> entityBodies;
    
    void createPlayerBody(Entity entity);
    void createBulletBody(Entity entity);
    void createObstacleBody(Entity entity);
    void syncPhysicsToECS();
    void syncECSToPhysics();
    void handleCollisions();
    
    // Helper functions
    b2Vec2 pixelsToMeters(float pixelX, float pixelY);
    void metersToPixels(const b2Vec2& meters, float& pixelX, float& pixelY);
    void handleBulletObstacleCollision(Entity bullet, Entity obstacle);
};

/**
 * @brief Contact listener for Box2D collision detection
 */
class ContactListener : public b2ContactListener
{
public:
    ContactListener(PhysicsSystem* physics) : physicsSystem(physics) {}
    
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
    
private:
    PhysicsSystem* physicsSystem;
};
