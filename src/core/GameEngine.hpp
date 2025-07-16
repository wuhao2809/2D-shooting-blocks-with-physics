#pragma once
#include "Manager.hpp"
#include "Blackboard.hpp"
#include "../input/InputSystem.hpp"
#include "../movement/MovementSystem.hpp"
#include "../gameplay/ShootingSystem.hpp"
#include "../physics/PhysicsSystem.hpp"
#include "../map/MapSystem.hpp"
#include "../rendering/RenderingSystem.hpp"
#include "../rendering/HUDSystem.hpp"
#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <string>

/**
 * @brief Main game engine class that manages all systems and game loop
 */
class GameEngine
{
public:
    GameEngine();
    ~GameEngine();

    bool initialize();
    void run();
    void shutdown();

    // System access
    Manager *getManager() { return &manager; }
    InputSystem *getInputSystem() { return &inputSystem; }
    MovementSystem *getMovementSystem() { return movementSystem.get(); }
    ShootingSystem *getShootingSystem() { return shootingSystem.get(); }
    PhysicsSystem *getPhysicsSystem() { return physicsSystem.get(); }
    MapSystem *getMapSystem() { return mapSystem.get(); }
    RenderingSystem *getRenderingSystem() { return renderingSystem.get(); }
    HUDSystem *getHUDSystem() { return hudSystem.get(); }
    Blackboard *getBlackboard() { return &blackboard; }

private:
    // Core systems
    Manager manager;
    Blackboard blackboard;
    InputSystem inputSystem;
    std::unique_ptr<MovementSystem> movementSystem;
    std::unique_ptr<ShootingSystem> shootingSystem;
    std::unique_ptr<PhysicsSystem> physicsSystem;
    std::unique_ptr<MapSystem> mapSystem;
    std::unique_ptr<RenderingSystem> renderingSystem;
    std::unique_ptr<HUDSystem> hudSystem;

    // SDL components
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    // Game state
    bool running = false;
    Uint32 lastTicks = 0;

    // Configuration
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;
    static constexpr const char *WINDOW_TITLE = "2D Shooter Prototype";

    // Private methods
    bool initializeSDL();
    bool loadGameData();
    void handleEvents();
    void update(float dt);
    void render();
    void createEntityFromJSON(const nlohmann::json &entityData);
};
