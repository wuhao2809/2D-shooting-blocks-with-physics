# 2D Top-Down Shooting Game - Project Design Documentation

## Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [System Flow](#system-flow)
4. [Player Movement System](#player-movement-system)
5. [Shooting System](#shooting-system)
6. [Collision Detection System](#collision-detection-system)
7. [Physics System](#physics-system)
8. [System Integration](#system-integration)
9. [Data Flow Diagrams](#data-flow-diagrams)
10. [Key Components](#key-components)

---

## Overview

This is a 2D top-down shooting game built with C++ and SDL3, featuring:

- Entity-Component-System (ECS) architecture
- Box2D 3.x physics engine integration
- Real-time collision detection
- Modular system design with blackboard communication
- HUD system with FPS display
- Dynamic obstacle interactions

---

## Architecture

### Core Components

The project uses an **Entity-Component-System (ECS)** architecture with the following key elements:

#### 1. **Entity Management**

- **Entity**: Simple integer ID (typedef uint32_t)
- **Manager**: Handles entity creation, deletion, and component management
- **Components**: Plain data structures (Position, Velocity, Renderable, etc.)

#### 2. **Communication System**

- **Blackboard**: Central message-passing system using std::any for type-safe communication
- **Systems**: Independent modules that process specific aspects of gameplay

#### 3. **Rendering System**

- **SDL3**: Graphics rendering and window management
- **Custom Renderer**: Handles sprite rendering, HUD display, and visual effects

---

## System Flow

### Main Execution Flow

```
main()
  → GameEngine::initialize()
    → initializeSDL()
    → Create all systems (Input, Movement, Shooting, Physics, Rendering, HUD, Map)
    → Setup blackboard connections
    → Load game data from JSON
    → Load map and create entities
  → GameEngine::run()
    → Game Loop:
      → handleEvents()
      → update(dt)
        → inputSystem.update()
        → movementSystem.update()
        → shootingSystem.update()
        → physicsSystem.update()
        → mapSystem.update()
        → renderingSystem.update()
        → hudSystem.update()
      → render()
        → renderingSystem.render()
        → hudSystem.render()
```

### System Update Order

The systems are updated in a specific order each frame to ensure proper data flow:

1. **InputSystem** - Captures user input and posts requests to blackboard
2. **MovementSystem** - Processes movement requests and updates positions
3. **ShootingSystem** - Handles bullet creation and lifecycle
4. **PhysicsSystem** - Manages Box2D world simulation and collisions
5. **MapSystem** - Handles map-related updates
6. **RenderingSystem** - Prepares rendering data
7. **HUDSystem** - Updates HUD elements

---

## Player Movement System

### Input Processing Flow

```
User Input (WASD keys)
  ↓
InputSystem::handleEvent()
  → Captures SDL_EVENT_KEY_DOWN/UP
  → Updates keyStates[entity][scancode]
  ↓
InputSystem::update()
  → Checks keyStates for movement keys
  → Calculates movement direction (moveX, moveY)
  → Posts to blackboard:
    - "movement_request" = true
    - "movement_entity" = player entity
    - "movement_x" = horizontal velocity
    - "movement_y" = vertical velocity
  → Calls updatePlayerDirection()
  ↓
MovementSystem::update()
  → Reads blackboard movement request
  → Updates entity's Velocity component
  → Calls applyVelocityEffects() for all entities (friction/damping)
  → Applies velocity-based boundary constraints
  ↓
PhysicsSystem::syncECSToPhysics()
  → Reads ECS Velocity components
  → Updates Box2D body velocities
  ↓
PhysicsSystem::syncPhysicsToECS()
  → Reads Box2D body states
  → **Updates ECS Position components** (single source of truth)
```

### Key Functions

#### InputSystem::update()

- **Purpose**: Captures WASD input and converts to movement requests
- **Input**: Delta time, SDL key states
- **Output**: Blackboard messages for movement and shooting
- **Special Features**:
  - Diagonal movement support
  - Direction calculation (0-360 degrees)
  - Fire rate limiting

#### MovementSystem::update() (**UPDATED ARCHITECTURE** ✅)

- **Purpose**: Processes movement requests and applies velocity effects
- **Input**: Blackboard movement requests
- **Output**: Updated Velocity components only
- **Features**:
  - **No longer updates positions** - PhysicsSystem handles all position updates
  - Applies friction/damping to non-bullet entities
  - Velocity-based boundary constraints for player

### **Clean Architecture Benefits** ✅

The updated architecture now follows a **single responsibility principle**:

| System              | Responsibility                                           |
| ------------------- | -------------------------------------------------------- |
| **InputSystem**     | Captures user input → velocity requests                  |
| **MovementSystem**  | Processes velocity requests → modifies velocity          |
| **PhysicsSystem**   | Velocity → position integration (single source of truth) |
| **RenderingSystem** | Positions → visual display                               |

**Key Improvement**: Eliminates duplicate position calculations and ensures physics is the authoritative source for all position updates.

- Friction/damping for smooth movement
- Boundary constraints for player
- Different rules for bullets vs. player

---

## Shooting System

### Shooting Flow

```
User Input (SPACE key)
  ↓
InputSystem::handleShooting()
  → Posts to blackboard:
    - "shoot_request" = true
    - "shoot_entity" = shooter entity
    - "shoot_time" = current game time
  ↓
ShootingSystem::update()
  → Reads blackboard shoot request
  → Calls handleShoot()
  ↓
ShootingSystem::handleShoot()
  → Checks fire rate limitations
  → Gets shooter position and direction
  → Calls createBullet()
  ↓
ShootingSystem::createBullet()
  → Creates new bullet entity
  → Adds Position, Velocity, Renderable, Bullet components
  → Calculates bullet trajectory based on player direction
  → Registers bullet with MovementSystem
  → Posts physics registration request to blackboard
  ↓
PhysicsSystem::addEntity()
  → Creates Box2D body for bullet
  → Sets up collision detection
  → Adds to physics simulation
```

### Key Functions

#### ShootingSystem::handleShoot()

- **Purpose**: Creates bullets when shooting is requested
- **Input**: Shooter entity, current time
- **Output**: New bullet entity with components
- **Features**:
  - Fire rate control (configurable bullets per second)
  - Direction-based bullet trajectory
  - Bullet lifetime management

#### ShootingSystem::updateBullets()

- **Purpose**: Updates all active bullets
- **Features**:
  - Position updates based on velocity
  - Lifetime tracking
  - Out-of-bounds detection
  - Automatic cleanup

### Bullet Lifecycle & Removal

#### Current Bullet Removal Mechanisms

1. **Time-based Removal** (`removeExpiredBullets()`)

   - Bullets removed after 3 seconds (configurable lifetime)
   - Prevents infinite bullets in memory

2. **Boundary-based Removal** (`removeBulletsOutOfBounds()`)

   - Bullets removed when they leave screen bounds
   - Prevents bullets from continuing indefinitely

3. **Collision-based Removal** (**NOW IMPLEMENTED** ✅)
   - **Feature**: Bullets are removed when they hit obstacles
   - **Implementation**: ShootingSystem listens for "collision_event" on blackboard
   - **Behavior**: Bullets disappear immediately on obstacle impact

#### How Bullets Currently Handle Collisions (**FIXED** ✅)

```
Bullet hits obstacle:
  ↓
PhysicsSystem::handleBulletObstacleCollision()
  → Applies impulse to obstacle
  → **Immediately removes bullet from game**
  → Destroys bullet's physics body
  → Removes bullet from entity manager
  → Posts collision event to blackboard
  ↓
ShootingSystem::update()
  → Reads collision event
  → Cleans up bullet from entity list
  → **Bullet is completely removed** ✅
```

**Key Fix:** Bullet removal is now handled directly by the PhysicsSystem when collision is detected, eliminating the race condition that prevented bullets from being removed.

#### Implementation Details

```cpp
// In ShootingSystem::update():
if (blackboard->has("collision_event") && blackboard->getValue<bool>("collision_event"))
{
    Entity bullet = blackboard->getValue<Entity>("collision_bullet");
    removeBulletOnCollision(bullet);
    blackboard->setValue("collision_event", false);
}

// New method:
void ShootingSystem::removeBulletOnCollision(Entity bullet)
{
    manager->removeEntity(bullet);
    entities.erase(std::remove(entities.begin(), entities.end(), bullet), entities.end());
}
```

---

## Collision Detection System

### Collision Processing Flow

```
PhysicsSystem::update()
  ↓
PhysicsSystem::handleCollisions()
  → Bullet-Obstacle Collision Detection:
    → Iterate through all bullets
    → Check against all obstacles
    → Simple bounding box collision test
    → Call handleBulletObstacleCollision()
  → Player-Obstacle Collision Detection:
    → Iterate through all players
    → Check against all obstacles
    → Bounding box collision test (32x32 player vs obstacle size)
    → Call handlePlayerObstacleCollision()
  ↓
PhysicsSystem::handleBulletObstacleCollision()
  → Apply impulse to obstacle
  → Post collision event to blackboard:
    - "collision_event" = true
    - "collision_bullet" = bullet entity
    - "collision_obstacle" = obstacle entity
  ↓
PhysicsSystem::handlePlayerObstacleCollision()
  → Apply knockback to player
  → Apply counter-impulse to obstacle
  → Post player collision event to blackboard:
    - "player_collision_event" = true
    - "player_collision_player" = player entity
    - "player_collision_obstacle" = obstacle entity
  ↓
PhysicsSystem::handleBoundaryCollisions()
  → Check obstacle boundaries
  → Reverse velocity on collision
  → Apply damping
  → Post boundary collision events
```

### Collision Types

#### 1. Bullet-Obstacle Collisions

- **Detection**: Bounding box intersection
- **Response**:
  - Apply impulse to obstacle
  - Generate collision events
  - **Current Implementation**: Bullets are NOT removed on collision
  - **Missing Feature**: Bullet removal on collision

**How Bullet Removal Should Work:**

```
PhysicsSystem::handleBulletObstacleCollision()
  → Apply impulse to obstacle
  → Post collision event to blackboard
  → **Missing**: Request bullet removal
  ↓
ShootingSystem::update() **Should Listen For:**
  → Check blackboard for "bullet_removal_request"
  → Call removeBulletOnCollision(bulletEntity)
  → Remove bullet from entities list
  → Call manager->removeEntity(bullet)
```

**Current Bullet Removal Methods:**

- `ShootingSystem::removeBulletsOutOfBounds()` - Removes bullets outside screen
- `ShootingSystem::removeExpiredBullets()` - Removes bullets after lifetime expires
- **Missing**: `removeBulletOnCollision()` - Should remove bullets on obstacle impact

#### 2. Boundary Collisions

- **Detection**: Position vs. window boundaries
- **Response**:
  - Velocity reversal
  - Position correction
  - Damping application

#### 3. Player-Boundary Collisions

- **Detection**: Player position vs. screen edges
- **Response**:
  - Position clamping
  - Prevent movement beyond boundaries

#### 4. Player-Obstacle Collisions (**NEW FEATURE** ✅)

- **Detection**: Bounding box intersection between player (32x32) and obstacles
- **Response**:
  - **Player Knockback**: Player gets pushed away from obstacle
  - **Obstacle Counter-Push**: Obstacle receives smaller impulse in opposite direction
  - **Collision Events**: Posted to blackboard for other systems to handle

**Implementation Details:**

```cpp
void PhysicsSystem::handlePlayerObstacleCollision(Entity player, Entity obstacle)
{
    // Calculate collision direction (from obstacle to player)
    float dx = playerPos->x - obstaclePos->x;
    float dy = playerPos->y - obstaclePos->y;

    // Apply knockback to player
    float knockbackStrength = 150.0f;
    playerVel->x += dx * knockbackStrength;
    playerVel->y += dy * knockbackStrength;

    // Apply counter-impulse to obstacle
    float obstacleImpulse = 50.0f;
    obstacleVel->x -= dx * obstacleImpulse;
    obstacleVel->y -= dy * obstacleImpulse;

    // Post event to blackboard
    blackboard->setValue("player_collision_event", true);
}
```

**Blackboard Messages:**

- `"player_collision_event"` → bool
- `"player_collision_player"` → Entity
- `"player_collision_obstacle"` → Entity

---

## Physics System

### Box2D Integration

The physics system integrates Box2D 3.x for realistic physics simulation:

```
PhysicsSystem::PhysicsSystem()
  → Create Box2D world with zero gravity
  → Set up world parameters
  ↓
PhysicsSystem::addEntity()
  → Determine entity type (Player/Bullet/Obstacle)
  → Create appropriate Box2D body:
    - Player: Dynamic body with box shape
    - Bullet: Dynamic body with circle shape, sensor
    - Obstacle: Dynamic body with box shape
  ↓
PhysicsSystem::update()
  → syncECSToPhysics()
    → Update Box2D bodies from ECS components
  → b2World_Step()
    → Advance physics simulation
  → syncPhysicsToECS()
    → Update ECS components from Box2D bodies
  → handleCollisions()
    → Custom collision detection
  → handleBoundaryCollisions()
    → Boundary constraint enforcement
```

### Key Physics Functions

#### PhysicsSystem::syncECSToPhysics()

- **Purpose**: Updates Box2D world from ECS component data
- **Process**:
  - Read Position/Velocity components
  - Convert pixels to meters
  - Update Box2D body transforms and velocities

#### PhysicsSystem::syncPhysicsToECS()

- **Purpose**: Updates ECS components from Box2D simulation results
- **Process**:
  - Read Box2D body positions and velocities
  - Convert meters to pixels
  - Update Position/Velocity components

#### Unit Conversion

- **METERS_PER_PIXEL**: 1.0f / 30.0f (30 pixels = 1 meter)
- **PIXELS_PER_METER**: 30.0f
- **Purpose**: Box2D works in meters, game works in pixels

---

## System Integration

### Blackboard Communication

The blackboard system enables loose coupling between systems:

```cpp
// Common blackboard messages:

// Movement
"movement_request" -> bool
"movement_entity" -> Entity
"movement_x" -> float
"movement_y" -> float

// Shooting
"shoot_request" -> bool
"shoot_entity" -> Entity
"shoot_time" -> float

// Physics
"physics_new_entity_request" -> bool
"physics_new_entity" -> Entity
"collision_event" -> bool
"collision_bullet" -> Entity
"collision_obstacle" -> Entity

// HUD
"hud_toggle_request" -> bool
"exit_game_request" -> bool
```

### System Dependencies

```
InputSystem
  ↓ (blackboard)
MovementSystem ← → PhysicsSystem
  ↓ (blackboard)    ↓ (ECS components)
ShootingSystem ← → RenderingSystem
  ↓ (blackboard)    ↓ (visual feedback)
PhysicsSystem ← → HUDSystem
```

---

## Data Flow Diagrams

### Player Movement Data Flow

```
[User Input] → [InputSystem] → [Blackboard] → [MovementSystem] → [ECS Components] → [PhysicsSystem] → [Box2D] → [PhysicsSystem] → [ECS Components] → [RenderingSystem] → [Screen]
```

### Shooting Data Flow

```
[User Input] → [InputSystem] → [Blackboard] → [ShootingSystem] → [New Bullet Entity] → [PhysicsSystem] → [Box2D] → [Collision Detection] → [Blackboard] → [Game Response]
```

### Bullet Removal Data Flow (Current Implementation)

```
[Bullet Created] → [Time Tracking] → [Lifetime Check] → [removeExpiredBullets()] → [Entity Removed]
                ↓
[Position Update] → [Bounds Check] → [removeBulletsOutOfBounds()] → [Entity Removed]
                ↓
[Collision Detected] → [Impulse Applied] → [Blackboard Event] → [**Bullet Continues**] ❌
```

### Bullet Removal Data Flow (Recommended Implementation)

```
[Bullet Created] → [Time Tracking] → [Lifetime Check] → [removeExpiredBullets()] → [Entity Removed]
                ↓
[Position Update] → [Bounds Check] → [removeBulletsOutOfBounds()] → [Entity Removed]
                ↓
[Collision Detected] → [Impulse Applied] → [Blackboard Event] → [ShootingSystem] → [removeBulletOnCollision()] → [Entity Removed] ✅
```

### Collision Detection Data Flow

```
[PhysicsSystem] → [Bullet Position] → [Obstacle Position] → [Bounding Box Test] → [Collision Response] → [Velocity Update] → [Blackboard Event] → [Other Systems]
```

---

## Key Components

### Entity Components

```cpp
// Core position and movement
Position { float x, y }
Velocity { float x, y }

// Rendering
Renderable { string color, int width, height, bool showDirection }

// Input handling
Input { bool controllable }

// Directional entities
Direction { float angle }  // 0-360 degrees

// Shooting capability
Shooter { float fireRate, lastShotTime, bool canShoot }

// Bullet properties
Bullet { float speed, lifetime, timeAlive }
```

### System Responsibilities

| System          | Primary Function                             | Key Methods                                                        |
| --------------- | -------------------------------------------- | ------------------------------------------------------------------ |
| InputSystem     | Capture user input, convert to game commands | `handleEvent()`, `update()`, `handleShooting()`                    |
| MovementSystem  | Process movement requests, update positions  | `update()`, `updateEntityPosition()`, `applyBoundaryConstraints()` |
| ShootingSystem  | Handle bullet creation and lifecycle         | `handleShoot()`, `createBullet()`, `updateBullets()`               |
| PhysicsSystem   | Simulate physics, handle collisions          | `update()`, `syncECSToPhysics()`, `handleCollisions()`             |
| RenderingSystem | Draw game entities to screen                 | `render()`, `drawEntity()`                                         |
| HUDSystem       | Display game information                     | `render()`, `renderFPS()`, `toggleVisibility()`                    |

### Performance Considerations

1. **ECS Architecture**: Enables efficient component iteration and cache-friendly data access
2. **Blackboard System**: Reduces coupling but requires careful message management
3. **Physics Simulation**: Box2D provides optimized collision detection and physics
4. **Frame Rate**: Target 60 FPS with 16ms frame time
5. **Memory Management**: Smart pointers and RAII for automatic cleanup

---

## Conclusion

This 2D shooting game demonstrates a well-structured, modular approach to game development using modern C++ features and proven architectural patterns. The ECS architecture combined with blackboard communication provides a flexible and maintainable foundation for game development, while the integration with Box2D ensures realistic physics simulation.

The system design allows for easy extension and modification, making it suitable for adding new features like different weapon types, enemy AI, power-ups, or additional game mechanics.
