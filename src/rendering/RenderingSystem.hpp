#pragma once
#include "../core/System.hpp"
#include "../core/Components.hpp"
#include "Renderer.hpp"
#include <memory>

class Manager; // Forward declaration

/**
 * @brief ECS System for rendering entities
 */
class RenderingSystem : public System
{
public:
    RenderingSystem(SDL_Renderer *sdlRenderer, Manager *manager);
    ~RenderingSystem();

    void update(float dt) override;
    void render();

    // Access to renderer for direct rendering needs
    Renderer *getRenderer() { return renderer.get(); }

private:
    Manager *manager;
    std::unique_ptr<Renderer> renderer;
};
