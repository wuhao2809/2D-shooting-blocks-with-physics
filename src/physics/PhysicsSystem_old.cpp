#include "PhysicsSystem.hpp"
#include "../core/Manager.hpp"
#include <iostream>

PhysicsSystem::PhysicsSystem()
{
    // Create Box2D world with gravity
    b2Vec2 gravity(0.0f, 0.0f); // No gravity for top-down game
    world = std::make_unique<b2World>(gravity);
    
    // Set up contact listener for collision detection
    auto contactListener = std::make_unique<ContactListener>(this);
    world->SetContactListener(contactListener.get());
    
    std::cout << "[PhysicsSystem] Initialized with Box2D" << std::endl;
}

PhysicsSystem::~PhysicsSystem()
{
    // Clean up all bodies
    for (auto& pair : entityBodies) {
        if (pair.second) {
            world->DestroyBody(pair.second);
        }
    }
    entityBodies.clear();
    
    std::cout << "[PhysicsSystem] Destroyed" << std::endl;
}

void PhysicsSystem::update(float dt)
{
    // Check for new entities to add to physics world
    for (Entity entity : entities) {
        if (entityBodies.find(entity) == entityBodies.end()) {
            addEntity(entity);
        }
    }
    
    // Sync ECS components to physics bodies
    syncECSToPhysics();
    
    // Step the physics world
    int32 velocityIterations = 6;
    int32 positionIterations = 2;
    world->Step(dt, velocityIterations, positionIterations);
    
    // Sync physics bodies back to ECS components
    syncPhysicsToECS();
    
    // Handle collisions and physics events
    handleCollisions();
}

void PhysicsSystem::addEntity(Entity entity)
{
    Position* pos = getComponent<Position>(entity);
    if (!pos) return;
    
    // Determine entity type and create appropriate body
    Input* input = getComponent<Input>(entity);
    Bullet* bullet = getComponent<Bullet>(entity);
    
    if (input && input->controllable) {
        createPlayerBody(entity);
    } else if (bullet) {
        createBulletBody(entity);
    } else {
        // Check if it's an obstacle (has Position and Renderable but no Input/Bullet)
        Renderable* renderable = getComponent<Renderable>(entity);
        if (renderable) {
            createObstacleBody(entity);
        }
    }
}

void PhysicsSystem::removeEntity(Entity entity)
{
    auto it = entityBodies.find(entity);
    if (it != entityBodies.end()) {
        world->DestroyBody(it->second);
        entityBodies.erase(it);
    }
}

void PhysicsSystem::createPlayerBody(Entity entity)
{
    Position* pos = getComponent<Position>(entity);
    if (!pos) return;
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pixelsToMeters(pos->x + 16, pos->y + 16); // Center of 32x32 player
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(entity);
    
    b2Body* body = world->CreateBody(&bodyDef);
    
    // Create box shape for player
    b2PolygonShape shape;
    shape.SetAsBox(16 * METERS_PER_PIXEL, 16 * METERS_PER_PIXEL);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.1f;
    
    body->CreateFixture(&fixtureDef);
    entityBodies[entity] = body;
    
    std::cout << "[PhysicsSystem] Created player body for entity " << entity << std::endl;
}

void PhysicsSystem::createBulletBody(Entity entity)
{
    Position* pos = getComponent<Position>(entity);
    if (!pos) return;
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pixelsToMeters(pos->x + 4, pos->y + 4); // Center of 8x8 bullet
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(entity);
    bodyDef.bullet = true; // Enable continuous collision detection
    
    b2Body* body = world->CreateBody(&bodyDef);
    
    // Create circle shape for bullet
    b2CircleShape shape;
    shape.m_radius = 4 * METERS_PER_PIXEL;
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 0.1f;
    fixtureDef.friction = 0.0f;
    fixtureDef.restitution = 0.8f;
    fixtureDef.isSensor = true; // Bullets are sensors for now
    
    body->CreateFixture(&fixtureDef);
    entityBodies[entity] = body;
    
    std::cout << "[PhysicsSystem] Created bullet body for entity " << entity << std::endl;
}

void PhysicsSystem::createObstacleBody(Entity entity)
{
    Position* pos = getComponent<Position>(entity);
    Renderable* renderable = getComponent<Renderable>(entity);
    if (!pos || !renderable) return;
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody; // Make obstacles movable when hit
    bodyDef.position = pixelsToMeters(pos->x + renderable->width/2, pos->y + renderable->height/2);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(entity);
    
    b2Body* body = world->CreateBody(&bodyDef);
    
    // Create box shape for obstacle
    b2PolygonShape shape;
    shape.SetAsBox(renderable->width/2 * METERS_PER_PIXEL, renderable->height/2 * METERS_PER_PIXEL);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 2.0f;
    fixtureDef.friction = 0.5f;
    fixtureDef.restitution = 0.3f;
    
    body->CreateFixture(&fixtureDef);
    entityBodies[entity] = body;
    
    std::cout << "[PhysicsSystem] Created obstacle body for entity " << entity << std::endl;
}

void PhysicsSystem::syncECSToPhysics()
{
    for (auto& pair : entityBodies) {
        Entity entity = pair.first;
        b2Body* body = pair.second;
        
        Velocity* vel = getComponent<Velocity>(entity);
        if (vel) {
            b2Vec2 velocity = pixelsToMeters(vel->x, vel->y);
            body->SetLinearVelocity(velocity);
        }
    }
}

void PhysicsSystem::syncPhysicsToECS()
{
    for (auto& pair : entityBodies) {
        Entity entity = pair.first;
        b2Body* body = pair.second;
        
        Position* pos = getComponent<Position>(entity);
        Velocity* vel = getComponent<Velocity>(entity);
        
        if (pos) {
            b2Vec2 position = body->GetPosition();
            float pixelX, pixelY;
            metersToPixels(position, pixelX, pixelY);
            
            // Adjust for entity center offset
            pos->x = pixelX - 16; // Assuming 32x32 entities
            pos->y = pixelY - 16;
        }
        
        if (vel) {
            b2Vec2 velocity = body->GetLinearVelocity();
            vel->x = velocity.x * PIXELS_PER_METER;
            vel->y = velocity.y * PIXELS_PER_METER;
        }
    }
}

void PhysicsSystem::handleCollisions()
{
    // Process collision events and post to blackboard
    for (b2Contact* contact = world->GetContactList(); contact; contact = contact->GetNext()) {
        if (contact->IsTouching()) {
            b2Body* bodyA = contact->GetFixtureA()->GetBody();
            b2Body* bodyB = contact->GetFixtureB()->GetBody();
            
            Entity entityA = static_cast<Entity>(bodyA->GetUserData().pointer);
            Entity entityB = static_cast<Entity>(bodyB->GetUserData().pointer);
            
            // Check for bullet-obstacle collision
            Bullet* bulletA = getComponent<Bullet>(entityA);
            Bullet* bulletB = getComponent<Bullet>(entityB);
            
            if (bulletA && !bulletB) {
                // Entity A is bullet, Entity B is obstacle
                handleBulletObstacleCollision(entityA, entityB);
            } else if (bulletB && !bulletA) {
                // Entity B is bullet, Entity A is obstacle
                handleBulletObstacleCollision(entityB, entityA);
            }
        }
    }
}

void PhysicsSystem::handleBulletObstacleCollision(Entity bullet, Entity obstacle)
{
    if (blackboard) {
        blackboard->setValue("collision_bullet", bullet);
        blackboard->setValue("collision_obstacle", obstacle);
        blackboard->setValue("collision_event", true);
        
        std::cout << "[PhysicsSystem] Bullet " << bullet << " hit obstacle " << obstacle << std::endl;
    }
}

b2Vec2 PhysicsSystem::pixelsToMeters(float pixelX, float pixelY)
{
    return b2Vec2(pixelX * METERS_PER_PIXEL, pixelY * METERS_PER_PIXEL);
}

void PhysicsSystem::metersToPixels(const b2Vec2& meters, float& pixelX, float& pixelY)
{
    pixelX = meters.x * PIXELS_PER_METER;
    pixelY = meters.y * PIXELS_PER_METER;
}

// ContactListener implementation
void ContactListener::BeginContact(b2Contact* contact)
{
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    
    Entity entityA = static_cast<Entity>(bodyA->GetUserData().pointer);
    Entity entityB = static_cast<Entity>(bodyB->GetUserData().pointer);
    
    std::cout << "[ContactListener] Collision between entity " << entityA << " and " << entityB << std::endl;
}

void ContactListener::EndContact(b2Contact* contact)
{
    // Handle collision end if needed
}
