#include "RenderingSystem.hpp"
#include "../core/Manager.hpp"
#include <iostream>

RenderingSystem::RenderingSystem(SDL_Renderer *sdlRenderer, Manager *mgr)
    : manager(mgr), renderer(std::make_unique<Renderer>(sdlRenderer))
{
    std::cout << "[RenderingSystem] Initialized" << std::endl;
}

RenderingSystem::~RenderingSystem()
{
    std::cout << "[RenderingSystem] Destroyed" << std::endl;
}

void RenderingSystem::update(float dt)
{
    // RenderingSystem update logic can be added here if needed
    // For now, main rendering happens in render()
}

void RenderingSystem::render()
{
    renderer->beginFrame();

    // Get all entities from manager
    std::vector<Entity> allEntities = manager->getAllEntities();

    // Render all entities with Position and Renderable components
    renderer->renderAllEntities(allEntities);

    renderer->endFrame();
}
