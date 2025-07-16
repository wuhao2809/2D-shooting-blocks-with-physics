#pragma once
#include "../core/Components.hpp"
#include "../core/Entity.hpp"
#include <SDL3/SDL.h>
#include <vector>

/**
 * @brief Main rendering system that handles all SDL drawing operations
 */
class Renderer
{
public:
    Renderer(SDL_Renderer *sdlRenderer);
    ~Renderer();

    // Core rendering methods
    void beginFrame();
    void endFrame();
    void clear();

    // Entity rendering
    void renderEntity(Entity entity, const Position &pos, const Renderable &renderable);
    void renderAllEntities(const std::vector<Entity> &entities);

    // Specialized rendering
    void renderDirectionLine(const Position &pos, const Direction &dir, int entityWidth, int entityHeight);
    void renderBullet(const Position &pos, const Renderable &renderable);
    void renderPlayer(const Position &pos, const Renderable &renderable, const Direction *dir = nullptr);

    // Utility methods
    void setColor(const std::string &colorName);
    void setColor(int r, int g, int b, int a = 255);

private:
    SDL_Renderer *renderer;

    // Color mapping
    void parseColor(const std::string &colorName, int &r, int &g, int &b, int &a);
};
