#include "PhysicsSystem_new.hpp"
#include "../core/Manager.hpp"
#include "../core/Components.hpp"
#include <iostream>

PhysicsSystem::PhysicsSystem()
{
    // Create Box2D world with default settings
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 0.0f}; // No gravity for top-down shooter
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

    // Sync ECS data to physics world
    syncECSToPhysics();

    // Step the physics world
    b2World_Step(worldId, dt, 4); // 4 sub-steps for stability

    // Handle collision events
    processCollisionEvents();

    // Check for physics impulse requests from blackboard
    if (blackboard && blackboard->has("physics_impulse_request") &&
        blackboard->getValue<bool>("physics_impulse_request"))
    {

        Entity entity = blackboard->getValue<Entity>("physics_impulse_entity");
        float impulseX = blackboard->getValue<float>("physics_impulse_x");
        float impulseY = blackboard->getValue<float>("physics_impulse_y");

        auto it = entityBodies.find(entity);
        if (it != entityBodies.end())
        {
            b2Vec2 impulse = {impulseX * METERS_PER_PIXEL, impulseY * METERS_PER_PIXEL};
            b2Body_ApplyLinearImpulseToCenter(it->second, impulse, true);
        }

        blackboard->setValue("physics_impulse_request", false);
    }

    // Sync physics world back to ECS
    syncPhysicsToECS();
}

void PhysicsSystem::addEntity(Entity entity)
{
    Manager *manager = Manager::getInstance();

    // Determine entity type and create appropriate body
    if (manager->hasComponent<Player>(entity))
    {
        createPlayerBody(entity);
    }
    else if (manager->hasComponent<Bullet>(entity))
    {
        createBulletBody(entity);
    }
    else if (manager->hasComponent<Position>(entity) &&
             manager->hasComponent<Renderable>(entity))
    {
        createObstacleBody(entity);
    }
}

void PhysicsSystem::removeEntity(Entity entity)
{
    auto it = entityBodies.find(entity);
    if (it != entityBodies.end())
    {
        if (b2Body_IsValid(it->second))
        {
            b2DestroyBody(it->second);
        }
        entityBodies.erase(it);
    }
}

void PhysicsSystem::createPlayerBody(Entity entity)
{
    Manager *manager = Manager::getInstance();
    if (!manager->hasComponent<Position>(entity))
        return;

    Position *pos = manager->getComponent<Position>(entity);
    b2Vec2 position = pixelsToMeters(pos->x, pos->y);

    // Create body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.linearDamping = 2.0f; // Smooth movement
    bodyDef.userData = (void *)(uintptr_t)entity;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    // Create circular shape for player
    b2Circle circle = {{0.0f, 0.0f}, 0.5f}; // 0.5 meter radius
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;

    b2CreateCircleShape(bodyId, &shapeDef, &circle);

    entityBodies[entity] = bodyId;
}

void PhysicsSystem::createBulletBody(Entity entity)
{
    Manager *manager = Manager::getInstance();
    if (!manager->hasComponent<Position>(entity))
        return;

    Position *pos = manager->getComponent<Position>(entity);
    b2Vec2 position = pixelsToMeters(pos->x, pos->y);

    // Create body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.isBullet = true; // Enable continuous collision detection
    bodyDef.userData = (void *)(uintptr_t)entity;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    // Create small circular shape for bullet
    b2Circle circle = {{0.0f, 0.0f}, 0.1f}; // 0.1 meter radius
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.1f;
    shapeDef.friction = 0.0f;
    shapeDef.isSensor = true; // Bullets are sensors for collision detection

    b2CreateCircleShape(bodyId, &shapeDef, &circle);

    entityBodies[entity] = bodyId;
}

void PhysicsSystem::createObstacleBody(Entity entity)
{
    Manager *manager = Manager::getInstance();
    if (!manager->hasComponent<Position>(entity) || !manager->hasComponent<Renderable>(entity))
        return;

    Position *pos = manager->getComponent<Position>(entity);
    Renderable *renderable = manager->getComponent<Renderable>(entity);

    b2Vec2 position = pixelsToMeters(pos->x, pos->y);

    // Create body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    bodyDef.userData = (void *)(uintptr_t)entity;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    // Create box shape for obstacle
    b2Vec2 size = {renderable->width * METERS_PER_PIXEL * 0.5f,
                   renderable->height * METERS_PER_PIXEL * 0.5f};
    b2Polygon box = b2MakeBox(size.x, size.y);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.5f;
    shapeDef.restitution = 0.2f;

    b2CreatePolygonShape(bodyId, &shapeDef, &box);

    entityBodies[entity] = bodyId;
}

void PhysicsSystem::syncPhysicsToECS()
{
    Manager *manager = Manager::getInstance();

    for (auto &[entity, bodyId] : entityBodies)
    {
        if (!b2Body_IsValid(bodyId))
            continue;

        if (manager->hasComponent<Position>(entity))
        {
            Position *pos = manager->getComponent<Position>(entity);
            b2Vec2 position = b2Body_GetPosition(bodyId);
            metersToPixels(position, pos->x, pos->y);
        }

        if (manager->hasComponent<Velocity>(entity))
        {
            Velocity *vel = manager->getComponent<Velocity>(entity);
            b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);
            vel->x = velocity.x * PIXELS_PER_METER;
            vel->y = velocity.y * PIXELS_PER_METER;
        }
    }
}

void PhysicsSystem::syncECSToPhysics()
{
    Manager *manager = Manager::getInstance();

    for (auto &[entity, bodyId] : entityBodies)
    {
        if (!b2Body_IsValid(bodyId))
            continue;

        if (manager->hasComponent<Velocity>(entity))
        {
            Velocity *vel = manager->getComponent<Velocity>(entity);
            b2Vec2 velocity = {vel->x * METERS_PER_PIXEL, vel->y * METERS_PER_PIXEL};
            b2Body_SetLinearVelocity(bodyId, velocity);
        }
    }
}

void PhysicsSystem::processCollisionEvents()
{
    // Box2D 3.x uses contact events instead of listeners
    b2ContactEvents contactEvents = b2World_GetContactEvents(worldId);

    // Process begin contact events
    for (int i = 0; i < contactEvents.beginCount; ++i)
    {
        b2ContactBeginTouchEvent *beginEvent = contactEvents.beginEvents + i;

        Entity entityA = (Entity)(uintptr_t)b2Body_GetUserData(beginEvent->bodyIdA);
        Entity entityB = (Entity)(uintptr_t)b2Body_GetUserData(beginEvent->bodyIdB);

        handleBulletObstacleCollision(entityA, entityB);
        handleBulletObstacleCollision(entityB, entityA);
    }
}

void PhysicsSystem::handleBulletObstacleCollision(Entity bullet, Entity obstacle)
{
    Manager *manager = Manager::getInstance();

    if (manager->hasComponent<Bullet>(bullet) &&
        manager->hasComponent<Renderable>(obstacle))
    {

        // Signal collision to blackboard
        if (blackboard)
        {
            blackboard->setValue("collision_event", true);
            blackboard->setValue("collision_bullet", bullet);
            blackboard->setValue("collision_obstacle", obstacle);
        }

        // Remove bullet entity
        removeEntity(bullet);
        manager->removeEntity(bullet);

        std::cout << "[PhysicsSystem] Bullet " << bullet << " hit obstacle " << obstacle << std::endl;
    }
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
