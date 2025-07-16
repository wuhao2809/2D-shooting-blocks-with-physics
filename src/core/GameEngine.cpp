#include "GameEngine.hpp"
#include "Components.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unistd.h>

GameEngine::GameEngine()
{
    std::cout << "[GameEngine] Created" << std::endl;
}

GameEngine::~GameEngine()
{
    shutdown();
    std::cout << "[GameEngine] Destroyed" << std::endl;
}

bool GameEngine::initialize()
{
    std::cout << "[GameEngine] Initializing..." << std::endl;

    if (!initializeSDL())
    {
        return false;
    }

    // Create rendering system
    renderingSystem = std::make_unique<RenderingSystem>(renderer, &manager);

    // Create movement system
    movementSystem = std::make_unique<MovementSystem>();

    // Create shooting system
    shootingSystem = std::make_unique<ShootingSystem>(&manager);

    // Create physics system
    // physicsSystem = std::make_unique<PhysicsSystem>();  // Temporarily disabled

    // Create map system
    mapSystem = std::make_unique<MapSystem>(&manager);

    // Setup blackboard for all systems
    inputSystem.setBlackboard(&blackboard);
    movementSystem->setBlackboard(&blackboard);
    shootingSystem->setBlackboard(&blackboard);
    // physicsSystem->setBlackboard(&blackboard);  // Temporarily disabled
    mapSystem->setBlackboard(&blackboard);
    renderingSystem->setBlackboard(&blackboard);

    std::cout << "[GameEngine] Blackboard setup complete" << std::endl;

    if (!loadGameData())
    {
        return false;
    }

    // Load map
    if (!mapSystem->loadMap("assets/map1.json"))
    {
        std::cerr << "[GameEngine] Failed to load map" << std::endl;
        return false;
    }

    // Create map entities
    mapSystem->createMapEntities();

    std::cout << "[GameEngine] Map loaded successfully" << std::endl;

    running = true;
    lastTicks = SDL_GetTicks();

    std::cout << "[GameEngine] Initialization complete" << std::endl;
    return true;
}

bool GameEngine::initializeSDL()
{
    // Print current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr)
    {
        std::cout << "[GameEngine] Current working dir: " << cwd << std::endl;
    }

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "[GameEngine] SDL_Init Error: '" << SDL_GetError() << "'" << std::endl;
        return false;
    }

    // Create window
    window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window)
    {
        std::cerr << "[GameEngine] SDL_CreateWindow Error: '" << SDL_GetError() << "'" << std::endl;
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "[GameEngine] SDL_CreateRenderer Error: '" << SDL_GetError() << "'" << std::endl;
        return false;
    }

    std::cout << "[GameEngine] SDL initialized successfully" << std::endl;
    return true;
}

bool GameEngine::loadGameData()
{
    std::cout << "[GameEngine] Loading game data..." << std::endl;

    std::ifstream file("gamedata.json");
    if (!file.is_open())
    {
        std::cerr << "[GameEngine] Failed to open gamedata.json" << std::endl;
        return false;
    }

    nlohmann::json data;
    file >> data;
    if (file.fail())
    {
        std::cerr << "[GameEngine] Failed to parse gamedata.json or file is empty." << std::endl;
        return false;
    }
    file.close();

    // Create entities from JSON
    for (const auto &entityData : data["entities"])
    {
        createEntityFromJSON(entityData);
    }

    std::cout << "[GameEngine] Game data loaded successfully" << std::endl;
    return true;
}

void GameEngine::createEntityFromJSON(const nlohmann::json &entityData)
{
    Entity entity = manager.createEntity();
    std::cout << "[GameEngine] Creating entity " << entity << " (" << entityData["name"] << ")" << std::endl;

    const auto &components = entityData["components"];

    if (components.contains("Position"))
    {
        Position pos{components["Position"]["x"], components["Position"]["y"]};
        addComponent<Position>(entity, pos);
        std::cout << "[GameEngine] Added Position component to entity " << entity << std::endl;
    }

    if (components.contains("Input"))
    {
        Input input{components["Input"]["controllable"]};
        addComponent<Input>(entity, input);
        inputSystem.entities.push_back(entity);
        std::cout << "[GameEngine] Added Input component to entity " << entity << std::endl;
    }

    if (components.contains("Renderable"))
    {
        Renderable renderable{
            components["Renderable"]["color"],
            components["Renderable"]["width"],
            components["Renderable"]["height"]};
        // Check if showDirection is specified
        if (components["Renderable"].contains("showDirection"))
        {
            renderable.showDirection = components["Renderable"]["showDirection"];
        }
        addComponent<Renderable>(entity, renderable);
        std::cout << "[GameEngine] Added Renderable component to entity " << entity << std::endl;
    }

    if (components.contains("Direction"))
    {
        Direction direction{components["Direction"]["angle"]};
        addComponent<Direction>(entity, direction);
        std::cout << "[GameEngine] Added Direction component to entity " << entity << std::endl;
    }

    if (components.contains("Shooter"))
    {
        Shooter shooter{
            components["Shooter"]["fireRate"],
            components["Shooter"]["lastShotTime"],
            components["Shooter"]["canShoot"]};
        addComponent<Shooter>(entity, shooter);
        std::cout << "[GameEngine] Added Shooter component to entity " << entity << std::endl;
    }
}

void GameEngine::run()
{
    std::cout << "[GameEngine] Starting game loop..." << std::endl;

    while (running)
    {
        handleEvents();

        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTicks) / 1000.0f;
        lastTicks = now;

        update(dt);
        render();

        SDL_Delay(16); // ~60 FPS
    }

    std::cout << "[GameEngine] Game loop ended" << std::endl;
}

void GameEngine::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            running = false;
        }
        inputSystem.handleEvent(event);
    }
}

void GameEngine::update(float dt)
{
    inputSystem.update(dt);
    movementSystem->update(dt);
    shootingSystem->update(dt);
    // physicsSystem->update(dt);  // Temporarily disabled
    mapSystem->update(dt);
    renderingSystem->update(dt);
}

void GameEngine::render()
{
    renderingSystem->render();
}

void GameEngine::shutdown()
{
    std::cout << "[GameEngine] Shutting down..." << std::endl;

    renderingSystem.reset();

    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
    std::cout << "[GameEngine] Shutdown complete" << std::endl;
}
