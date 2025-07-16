#pragma once
#include "../core/System.hpp"
#include "../core/Manager.hpp"
#include "../core/Components.hpp"
#include <vector>
#include <string>

struct MapObstacle
{
    float x, y;
    float width, height;
    int r, g, b; // Color
};

struct MapData
{
    int width, height;
    std::vector<MapObstacle> obstacles;
};

/**
 * @brief System to load and manage game maps with obstacles
 */
class MapSystem : public System
{
public:
    MapSystem(Manager *mgr);
    void update(float dt) override;

    bool loadMap(const std::string &mapFile);
    void createMapEntities();
    const MapData &getMapData() const { return mapData; }

private:
    Manager *manager;
    MapData mapData;
    std::vector<Entity> obstacleEntities;
    bool mapLoaded = false;

    void createObstacle(float x, float y, float width, float height, int r, int g, int b);
};
