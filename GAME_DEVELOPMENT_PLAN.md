# 2D Top-Down Shooting Game Development Plan

## Current Status

- **Project Initialized**: Directory structure, CMakeLists.txt, and main.cpp created. ✅
- **Tech Stack Chosen**: C++, SDL3 (rendering/input), Box2D (physics), ECS architecture, Blackboard System for inter-system communication. ✅
- **ECS Core & Blackboard System completed and active.** ✅
- **Input & Event System completed.** ✅
- **WSAD Movement Prototype completed and fully functional.** ✅
- **Shooting System completed and fully functional.** ✅
- **Rendering System completed with proper architecture.** ✅
- **GameEngine architecture implemented with proper dependency injection.** ✅
- **Blackboard System implemented and integrated across all systems.** ✅
- **Movement System implemented with blackboard integration.** ✅
- **Map System implemented with JSON-based obstacle loading.** ✅
- **Game is fully runnable with all basic features working.** ✅
- **Physics System temporarily disabled pending Box2D 3.x API integration.** ⚠️

---

## Project Structure

```
2d_shooting_game_with_obstacles/
│
├── src/
│   ├── core/           # ECS core (Entity, Component, System, Manager) + GameEngine
│   │   ├── Entity.hpp          # Entity type definition
│   │   ├── Component.hpp       # Component base class
│   │   ├── Components.hpp      # All component definitions
│   │   ├── System.hpp          # System base class
│   │   ├── Manager.hpp/.cpp    # ECS manager with component storage
│   │   ├── GameEngine.hpp/.cpp # Main game engine class
│   │   └── Blackboard.hpp      # Inter-system communication (future)
│   │
│   ├── rendering/      # Rendering system with proper SDL3 abstraction
│   │   ├── Renderer.hpp/.cpp     # Low-level rendering operations
│   │   └── RenderingSystem.hpp/.cpp # ECS rendering system
│   │
│   ├── input/          # Input handling (WSAD, space, etc.)
│   │   ├── InputSystem.hpp/.cpp  # Input processing and player movement
│   │
│   ├── gameplay/       # Game logic (shooting, bullets, etc.)
│   │   └── ShootingSystem.hpp/.cpp # Bullet creation and management
│   │
│   ├── events/         # Event system (input, game events, etc.)
│   ├── assets/         # Asset management (textures, sounds, etc.)
│   ├── physics/        # Physics system (Box2D integration, collision, etc.)
│   ├── map/            # Map system (obstacle layout, map loading)
│   ├── utils/          # Utility functions, math, helpers
│   └── main.cpp        # Entry point (simplified with GameEngine)
│
├── assets/             # Game assets (images, sounds, maps)
│
├── include/            # Public headers (if needed)
│
├── third_party/        # SDL3, Box2D, and other external libraries
│
├── build/              # Build output
│
├── gamedata.json       # Entity/component definitions for easy extensibility
│
├── CMakeLists.txt      # Build configuration
│
└── GAME_DEVELOPMENT_PLAN.md  # This plan
```

---

## Architecture Highlights

### ECS (Entity-Component-System) Architecture

- **Entities**: Simple integer IDs representing game objects
- **Components**: Data structures (Position, Renderable, Input, Direction, Shooter, Bullet, Velocity)
- **Systems**: Logic processors (InputSystem, ShootingSystem, RenderingSystem)
- **Manager**: Central ECS manager with component storage and entity management
- **Blackboard**: Inter-system communication hub using type-safe key-value storage

### Blackboard System

- **Decoupled Communication**: Systems communicate via shared blackboard instead of direct references
- **Type Safety**: Template-based getValue/setValue methods with automatic type casting
- **Event-Driven**: InputSystem posts shoot requests, ShootingSystem processes them asynchronously
- **Future-Ready**: Designed to support physics events, collision notifications, and complex game logic

### Input System

- **WSAD Movement**: Smooth movement with 200 pixels/second speed
- **Direction Updates**: Player direction follows movement (8-directional)
- **Space Bar Shooting**: Posts shoot requests to blackboard instead of direct system calls
- **Blackboard Integration**: Uses blackboard->setValue() for shoot_request, shoot_entity, shoot_time

### Shooting System

- **Blackboard Integration**: Reads shoot requests from blackboard in update() loop
- **Bullet Creation**: Dynamic bullet entities with Position, Velocity, Bullet, Renderable components
- **Fire Rate Control**: 2 bullets per second (configurable)
- **Bullet Physics**: 400 pixels/second velocity in facing direction
- **Bullet Cleanup**: Automatic removal when out of bounds or expired (3 second lifetime)
- **Asynchronous Processing**: Decoupled from InputSystem via blackboard communication

### Rendering System

- **Renderer**: Low-level SDL3 rendering operations with color management
- **RenderingSystem**: ECS system that renders all entities with Position+Renderable components
- **Direction Visualization**: White line showing player facing direction

### GameEngine Architecture

- **System Management**: Centralized initialization and update loop
- **Dependency Injection**: Clean system dependencies without globals
- **JSON Configuration**: Data-driven entity creation from gamedata.json

---

## Development Plan

### 1. Project Setup ✅

- [x] Initialize C++ project with CMake.
- [x] Integrate SDL3 and Box2D (add to `third_party/`).
- [x] Set up basic build and run pipeline.

### 2. ECS Core & Blackboard System ✅

- [x] Implement ECS core: Entity, Component, System, Manager.
- [x] Implement component storage with proper singleton pattern.
- [x] Add entity management (create, remove, getAllEntities).
- [x] Template-based component system (addComponent, getComponent).

### 3. Input & Event System ✅

- [x] Implement input system (WSAD for movement, space for shooting).
- [x] Implement event system for game events (shoot, hit, break, etc.).
- [x] Use `gamedata.json` for entity/component definitions.
- [x] Implement a simple prototype: player can move around with WSAD.
- [x] Add boundary checking to keep player within screen bounds.
- [x] Add directional movement with 8-way direction support.

### 4. Shooting System ✅

- [x] Implement bullet spawning system with Space bar.
- [x] Add Direction component for player facing direction.
- [x] Add Shooter component with configurable fire rate.
- [x] Add Bullet component with lifetime and speed.
- [x] Add Velocity component for bullet movement.
- [x] Implement bullet movement and physics.
- [x] Add bullet cleanup (out of bounds, expired).
- [x] Visual direction indicator (white line on player).

### 5. Rendering System ✅

- [x] Implement proper rendering architecture with Renderer class.
- [x] Create RenderingSystem as ECS system.
- [x] Support multiple entity types (player, bullets).
- [x] Color management system (blue player, yellow bullets).
- [x] Direction line rendering for player.
- [x] Proper SDL3 abstraction layer.

### 6. GameEngine Architecture ✅

- [x] Create GameEngine class for system management.
- [x] Implement proper dependency injection.
- [x] Clean system initialization and shutdown.
- [x] JSON-based entity loading.
- [x] Centralized game loop management.

### 7. Asset Management

- [ ] Implement asset loader for textures, sounds, etc.
- [ ] Add sprite/texture support to replace colored rectangles.
- [ ] Create simple player sprite with direction indicator.
- [ ] Add bullet sprite/animation.
- [ ] Organize assets folder structure.

### 8. Physics System

- [ ] Integrate Box2D for physics simulation.
- [ ] Implement collision detection and response.
- [ ] Add bullet-obstacle collision.
- [ ] Sync physics with ECS entities.

### 9. Map System

- [ ] Design map format (JSON, TMX, or custom).
- [ ] Implement map loader and parser.
- [ ] Place obstacles on the map.
- [ ] Add destructible obstacles.

### 10. Enhanced Gameplay

- [ ] Add enemy entities.
- [ ] Implement enemy AI and movement.
- [ ] Add collision between bullets and enemies.
- [ ] Add scoring system.
- [ ] Add win/lose conditions.

### 11. Polish & Optimization

- [ ] Add sound effects and music.
- [ ] Optimize performance (batch rendering, memory usage).
- [ ] Add particle effects for explosions.
- [ ] Add menus, UI, and settings.

### 12. Documentation

- [x] Document code and architecture.
- [x] Update this plan as development progresses.
- [ ] Add code comments and API documentation.

---

## Current Features

### ✅ Completed Features

1. **Player Movement**: WSAD keys move blue player box at 200 pixels/second
2. **Directional Facing**: Player faces movement direction (8 directions)
3. **Direction Visualization**: White line shows player facing direction
4. **Shooting System**: Space bar shoots yellow bullets in facing direction
5. **Fire Rate Control**: 2 bullets per second maximum
6. **Bullet Physics**: Bullets move at 400 pixels/second with 3 second lifetime
7. **Boundary Checking**: Player stays within 800x600 window bounds
8. **Bullet Cleanup**: Automatic removal of bullets when out of bounds or expired
9. **ECS Architecture**: Proper entity-component-system with component storage
10. **Rendering System**: Clean SDL3 rendering with proper color management
11. **GameEngine**: Centralized game management with dependency injection
12. **Blackboard System**: Implemented inter-system communication with type-safe key-value storage
13. **Decoupled Systems**: InputSystem and ShootingSystem communicate via blackboard
14. **Debug Output**: Comprehensive logging for debugging (can be toggled)
15. **Movement System**: Dedicated system for processing all entity movement via blackboard
16. **Map System**: JSON-based map loading with 5 destructible obstacles
17. **Game Loop**: Fully functional game loop with proper event handling
18. **Entity Management**: Dynamic entity creation and removal working correctly

### 🚧 Known Issues

- **Physics System**: Box2D 3.x integration temporarily disabled due to API changes. Box2D 3.x uses a C-style functional API instead of the C++ OOP API used in 2.x versions.
- **Visual Graphics**: Currently using colored rectangles instead of sprites/textures
- **No Collision Detection**: Without physics system, bullets pass through obstacles
- **Limited Interactions**: Obstacles are static and don't respond to bullets yet

- **W/A/S/D**: Move player (updates facing direction)
- **Space**: Shoot bullets in facing direction
- **Close Window**: Exit game

### 🏗️ Architecture Benefits

- **Modular Design**: Clean separation of concerns with blackboard communication
- **Extensible**: Easy to add new systems without modifying existing ones
- **Data-Driven**: Entity configuration via JSON
- **Maintainable**: Proper dependency injection and clean interfaces
- **Debuggable**: Comprehensive logging system with blackboard tracing
- **Decoupled**: Systems communicate via blackboard, not direct references
- **Future-Ready**: Blackboard system ready for physics events, AI, and complex game logic

---

## Notes

- Systems now communicate via blackboard rather than direct references for better decoupling.
- The blackboard system is ready for physics events, collision notifications, and AI systems.
- Use ECS for all game objects (player, bullets, obstacles, etc.).
- Prefer data-driven design for maps and assets.
- Keep code modular and maintainable for future LLM-assisted development.
- Use `gamedata.json` for entity/component definitions to allow easy extensibility.
- Rendering system properly abstracts SDL3 operations.
- GameEngine provides clean lifecycle management for all systems.
- Blackboard system provides type-safe key-value storage for inter-system communication.

---

## Next Priority

The next logical step would be **Physics System** integration. With the blackboard system in place, physics events (collisions, impulses, etc.) can be easily communicated between systems. The physics system would:

1. Listen for collision events via blackboard
2. Post collision results back to blackboard 
3. Allow bullet-obstacle and bullet-enemy interactions
4. Enable proper game mechanics for the shooting game

After that, **Asset Management** would enhance the visual appeal with proper sprites and textures.

- [x] Initialize C++ project with CMake.
- [x] Integrate SDL3 and Box2D (add to `third_party/`).
- [x] Set up basic build and run pipeline.

### 2. ECS Core & Blackboard System (Completed)

- [x] Implement ECS core: Entity, Component, System, Manager.
- [x] Implement Blackboard System for inter-system communication.

### 3. Input & Event System (Completed)

- [x] Implement input system (WSAD for movement, space for shooting).
- [x] Implement event system for game events (shoot, hit, break, etc.).
- [x] Use `gamedata.json` for entity/component definitions.
- [x] Implement a simple prototype: player can move around with WSAD.
- [x] Add boundary checking to keep player within screen bounds.

### 4. Asset Management

- [ ] Implement asset loader for textures, sounds, etc.
- [ ] Organize assets folder structure.

### 5. Rendering System

- [ ] Implement rendering system using SDL3.
- [ ] Render player, obstacles, bullets.
- [ ] Support for simple animations (optional).

### 6. Physics System

- [ ] Integrate Box2D for physics simulation.
- [ ] Implement collision detection and response.
- [ ] Sync physics with ECS entities.

### 7. Map System

- [ ] Design map format (JSON, TMX, or custom).
- [ ] Implement map loader and parser.
- [ ] Place obstacles on the map.

### 8. Gameplay Logic

- [ ] Implement player movement and shooting.
- [ ] Implement bullet logic (spawn, move, collide).
- [ ] Implement obstacle breaking (bullets destroy obstacles).
- [ ] Add scoring, win/lose conditions (optional).

### 9. Polish & Optimization

- [ ] Add sound effects and music.
- [ ] Optimize performance (batch rendering, memory usage).
- [ ] Add menus, UI, and settings (optional).

### 10. Documentation

- [ ] Document code and architecture.
- [ ] Update this plan as development progresses.

---

## Notes

- Each system should be as decoupled as possible, communicating via the Blackboard.
- Use ECS for all game objects (player, bullets, obstacles, etc.).
- Prefer data-driven design for maps and assets.
- Keep code modular and maintainable for future LLM-assisted development.
- Use `gamedata.json` for entity/component definitions to allow easy extensibility.
