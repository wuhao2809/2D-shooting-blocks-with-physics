# 2D Top-Down Shooting Game

A 2D top-down shooting game built with C++ and SDL3, featuring physics-based gameplay with obstacles and dynamic interactions.

## Features

- **Entity-Component-System (ECS) Architecture**: Clean, modular design for easy expansion
- **Box2D 3.x Physics**: Realistic physics simulation for bullets, obstacles, and collisions
- **Real-time Collision Detection**: Bullet-obstacle and player-obstacle interactions
- **HUD System**: FPS display with toggle functionality (press H to hide/show)
- **Dynamic Obstacles**: Obstacles react to bullet impacts with physics-based movement
- **Smooth Controls**: WASD movement with mouse-controlled shooting
- **Blackboard Communication**: Type-safe inter-system messaging

## Controls

- **WASD**: Move player
- **Mouse**: Aim and shoot
- **H**: Toggle HUD visibility
- **ESC**: Exit game

## Requirements

### Dependencies

- **SDL3**: Graphics and input handling
- **Box2D 3.x**: Physics engine
- **nlohmann/json**: JSON parsing for map data
- **CMake 3.10+**: Build system
- **C++17**: Language standard

### macOS Installation (Homebrew)

```bash
# Install dependencies
brew install sdl3 box2d nlohmann-json cmake

# Clone the repository
git clone <repository-url>
cd 2d_shooting_game_with_obstacles
```

## Building

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
make

# Run the game
./TopDownShooter
```

## Project Structure

```
├── src/
│   ├── main.cpp                    # Entry point
│   ├── core/                       # Core ECS components
│   │   ├── GameEngine.cpp/.hpp     # Main game engine
│   │   ├── Manager.cpp/.hpp        # Entity/component manager
│   │   ├── Blackboard.cpp/.hpp     # Inter-system communication
│   │   ├── Components.hpp          # Component definitions
│   │   └── System.hpp              # Base system class
│   ├── input/                      # Input handling
│   │   └── InputSystem.cpp/.hpp    # Keyboard/mouse input
│   ├── movement/                   # Player movement
│   │   └── MovementSystem.cpp/.hpp # Movement physics
│   ├── gameplay/                   # Game mechanics
│   │   └── ShootingSystem.cpp/.hpp # Bullet management
│   ├── physics/                    # Physics simulation
│   │   └── PhysicsSystem.cpp/.hpp  # Box2D integration
│   ├── rendering/                  # Graphics rendering
│   │   ├── RenderingSystem.cpp/.hpp # Main renderer
│   │   ├── HUDSystem.cpp/.hpp      # HUD rendering
│   │   └── Renderer.cpp/.hpp       # SDL3 wrapper
│   └── map/                        # Map/level system
│       └── MapSystem.cpp/.hpp      # Map loading & obstacles
├── assets/
│   └── map1.json                   # Level data
├── CMakeLists.txt                  # Build configuration
└── README.md                       # This file
```

## Architecture Overview

### Entity-Component-System (ECS)

The game uses a clean ECS architecture where:

- **Entities** are simple integer IDs
- **Components** are plain data structures (Position, Velocity, Renderable, etc.)
- **Systems** process entities with specific component combinations

### Key Systems

1. **InputSystem**: Handles keyboard/mouse input
2. **MovementSystem**: Processes player movement with friction and boundaries
3. **ShootingSystem**: Manages bullet creation, movement, and lifecycle
4. **PhysicsSystem**: Box2D integration for realistic physics simulation
5. **RenderingSystem**: SDL3-based graphics rendering
6. **HUDSystem**: User interface with FPS display
7. **MapSystem**: Level loading and obstacle management

### Communication

Systems communicate through a **Blackboard** pattern:

- Type-safe message passing using `std::any`
- Decoupled system interactions
- Event-driven architecture

## Physics Implementation

The game uses Box2D 3.x for realistic physics:

- **Velocity-Based Movement**: Systems modify velocities, physics handles positions
- **Collision Detection**: Automatic collision detection between bullets, player, and obstacles
- **Impulse Responses**: Realistic reactions to collisions
- **Boundary Constraints**: Obstacles bounce off screen edges

## Map Format

Maps are defined in JSON format:

```json
{
  "width": 800,
  "height": 600,
  "obstacles": [
    {
      "x": 300,
      "y": 200,
      "width": 100,
      "height": 50,
      "color": { "r": 139, "g": 69, "b": 19 }
    }
  ]
}
```

## Development

### Adding New Features

1. **New Component**: Add to `Components.hpp`
2. **New System**: Inherit from `System` base class
3. **Register System**: Add to `GameEngine.cpp`
4. **Communication**: Use blackboard for inter-system messaging

### Debugging

- Systems output debug information to console
- HUD shows real-time FPS
- Build with debug symbols: `cmake -DCMAKE_BUILD_TYPE=Debug ..`

## Performance

- Target: 60 FPS
- ECS architecture provides efficient entity processing
- Box2D handles physics optimization
- Minimal memory allocations during gameplay

## License

This project is for educational purposes. Dependencies have their own licenses:

- SDL3: zlib license
- Box2D: MIT license
- nlohmann/json: MIT license

## Contributing

1. Fork the repository
2. Create a feature branch
3. Follow the existing code style
4. Test your changes
5. Submit a pull request

## Troubleshooting

### Common Issues

1. **SDL3 not found**: Install with `brew install sdl3`
2. **Box2D not found**: Install with `brew install box2d`
3. **JSON header not found**: Install with `brew install nlohmann-json`
4. **Build fails**: Ensure CMake 3.10+ and C++17 support

### Performance Issues

- Check FPS in HUD (press H if hidden)
- Reduce number of obstacles in map file
- Ensure release build: `cmake -DCMAKE_BUILD_TYPE=Release ..`
