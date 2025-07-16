#include "MapSystem.hpp"
#include "../core/Manager.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

MapSystem::MapSystem(Manager *mgr) : manager(mgr)
{
    std::cout << "[MapSystem] Initialized" << std::endl;
}

void MapSystem::update(float dt)
{
    if (!mapLoaded)
        return;

    // Check for collision events from physics system
    if (blackboard && blackboard->has("collision_event") && blackboard->getValue<bool>("collision_event"))
    {
        Entity bullet = blackboard->getValue<Entity>("collision_bullet");
        Entity obstacle = blackboard->getValue<Entity>("collision_obstacle");

        // Apply impulse to obstacle when hit by bullet
        if (blackboard)
        {
            blackboard->setValue("physics_impulse_entity", obstacle);
            blackboard->setValue("physics_impulse_x", 100.0f); // Impulse strength
            blackboard->setValue("physics_impulse_y", 0.0f);
            blackboard->setValue("physics_impulse_request", true);
        }

        // Clear collision event
        blackboard->setValue("collision_event", false);

        std::cout << "[MapSystem] Obstacle " << obstacle << " hit by bullet " << bullet << std::endl;
    }
}

bool MapSystem::loadMap(const std::string &mapFile)
{
    std::ifstream file(mapFile);
    if (!file.is_open())
    {
        std::cerr << "[MapSystem] Failed to open map file: " << mapFile << std::endl;
        return false;
    }

    try
    {
        json j;
        file >> j;

        // Parse map dimensions
        mapData.width = j["width"].get<int>();
        mapData.height = j["height"].get<int>();

        // Parse obstacles
        mapData.obstacles.clear();
        for (const auto &obs : j["obstacles"])
        {
            MapObstacle obstacle;
            obstacle.x = obs["x"].get<float>();
            obstacle.y = obs["y"].get<float>();
            obstacle.width = obs["width"].get<float>();
            obstacle.height = obs["height"].get<float>();
            obstacle.r = obs["color"]["r"].get<int>();
            obstacle.g = obs["color"]["g"].get<int>();
            obstacle.b = obs["color"]["b"].get<int>();

            mapData.obstacles.push_back(obstacle);
        }

        mapLoaded = true;
        std::cout << "[MapSystem] Loaded map with " << mapData.obstacles.size() << " obstacles" << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[MapSystem] Error parsing map file: " << e.what() << std::endl;
        return false;
    }
}

void MapSystem::createMapEntities()
{
    if (!mapLoaded)
        return;

    for (const auto &obs : mapData.obstacles)
    {
        createObstacle(obs.x, obs.y, obs.width, obs.height, obs.r, obs.g, obs.b);
    }
}

void MapSystem::createObstacle(float x, float y, float width, float height, int r, int g, int b)
{
    Entity obstacle = manager->createEntity();

    // Add Position component
    Position pos;
    pos.x = x;
    pos.y = y;
    addComponent(obstacle, pos);

    // Add Renderable component
    Renderable renderable;
    renderable.width = width;
    renderable.height = height;
    renderable.color = "brown"; // Use string color for now
    addComponent(obstacle, renderable);

    // Add Velocity component for physics
    Velocity vel;
    vel.x = 0.0f;
    vel.y = 0.0f;
    addComponent(obstacle, vel);

    obstacleEntities.push_back(obstacle);
    entities.push_back(obstacle);

    std::cout << "[MapSystem] Created obstacle entity " << obstacle << " at (" << x << ", " << y << ")" << std::endl;
}
