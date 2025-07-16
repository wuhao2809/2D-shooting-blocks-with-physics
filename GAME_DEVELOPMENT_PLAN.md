# 2D Top-Down Shooting Game Development Plan

## Current Status

- **Project Initialized**: Directory structure, CMakeLists.txt, and main.cpp created.
- **Tech Stack Chosen**: C++, SDL3 (rendering/input), Box2D (physics), ECS architecture, Blackboard System for inter-system communication.
- **ECS Core & Blackboard System in progress.**

---

## Project Structure

```
2d_shooting_game_with_obstacles/
│
├── src/
│   ├── core/           # ECS core (Entity, Component, System, Manager, Blackboard)
│   ├── events/         # Event system (input, game events, etc.)
│   ├── assets/         # Asset management (textures, sounds, etc.)
│   ├── rendering/      # Rendering system (SDL3 wrappers, draw calls)
│   ├── physics/        # Physics system (Box2D integration, collision, etc.)
│   ├── map/            # Map system (obstacle layout, map loading)
│   ├── gameplay/       # Game logic (player, shooting, obstacle breaking)
│   ├── input/          # Input handling (WSAD, space, etc.)
│   ├── utils/          # Utility functions, math, helpers
│   └── main.cpp        # Entry point
│
├── assets/             # Game assets (images, sounds, maps)
│
├── include/            # Public headers (if needed)
│
├── third_party/        # SDL3, Box2D, and other external libraries
│
├── build/              # Build output
│
├── CMakeLists.txt      # Build configuration
│
└── GAME_DEVELOPMENT_PLAN.md  # This plan
```

---

## Development Plan

### 1. Project Setup

- [x] Initialize C++ project with CMake.
- [x] Integrate SDL3 and Box2D (add to `third_party/`).
- [x] Set up basic build and run pipeline.

### 2. ECS Core & Blackboard System (In Progress)

- [ ] Implement ECS core: Entity, Component, System, Manager.
- [ ] Implement Blackboard System for inter-system communication.

### 3. Input & Event System

- [ ] Implement input system (WSAD for movement, space for shooting).
- [ ] Implement event system for game events (shoot, hit, break, etc.).

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
