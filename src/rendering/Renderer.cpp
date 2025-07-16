#include "Renderer.hpp"
#include "../core/Manager.hpp"
#include <iostream>
#include <cmath>

Renderer::Renderer(SDL_Renderer *sdlRenderer) : renderer(sdlRenderer)
{
    std::cout << "[Renderer] Initialized" << std::endl;
}

Renderer::~Renderer()
{
    std::cout << "[Renderer] Destroyed" << std::endl;
}

void Renderer::beginFrame()
{
    clear();
}

void Renderer::endFrame()
{
    SDL_RenderPresent(renderer);
}

void Renderer::clear()
{
    setColor(30, 30, 30, 255); // Dark gray background
    SDL_RenderClear(renderer);
}

void Renderer::renderEntity(Entity entity, const Position &pos, const Renderable &renderable)
{
    SDL_FRect rect = {pos.x, pos.y, static_cast<float>(renderable.width), static_cast<float>(renderable.height)};

    setColor(renderable.color);
    SDL_RenderFillRect(renderer, &rect);

    // Render direction line if specified
    if (renderable.showDirection)
    {
        Direction *dir = getComponent<Direction>(entity);
        if (dir)
        {
            renderDirectionLine(pos, *dir, renderable.width, renderable.height);
        }
    }
}

void Renderer::renderAllEntities(const std::vector<Entity> &entities)
{
    for (const auto &entity : entities)
    {
        Position *pos = getComponent<Position>(entity);
        Renderable *renderable = getComponent<Renderable>(entity);

        if (!pos || !renderable)
            continue;

        renderEntity(entity, *pos, *renderable);
    }
}

void Renderer::renderDirectionLine(const Position &pos, const Direction &dir, int entityWidth, int entityHeight)
{
    // Calculate line from center of entity
    float centerX = pos.x + entityWidth / 2.0f;
    float centerY = pos.y + entityHeight / 2.0f;

    // Line length
    float lineLength = 20.0f;

    // Calculate end point
    float radians = dir.angle * M_PI / 180.0f;
    float endX = centerX + cos(radians) * lineLength;
    float endY = centerY + sin(radians) * lineLength;

    // Draw line
    setColor(255, 255, 255, 255); // White line
    SDL_RenderLine(renderer, centerX, centerY, endX, endY);
}

void Renderer::renderBullet(const Position &pos, const Renderable &renderable)
{
    renderEntity(0, pos, renderable); // Entity ID not needed for bullets
}

void Renderer::renderPlayer(const Position &pos, const Renderable &renderable, const Direction *dir)
{
    SDL_FRect rect = {pos.x, pos.y, static_cast<float>(renderable.width), static_cast<float>(renderable.height)};

    setColor(renderable.color);
    SDL_RenderFillRect(renderer, &rect);

    // Render direction line if direction is provided
    if (dir)
    {
        renderDirectionLine(pos, *dir, renderable.width, renderable.height);
    }
}

void Renderer::setColor(const std::string &colorName)
{
    int r, g, b, a;
    parseColor(colorName, r, g, b, a);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Renderer::setColor(int r, int g, int b, int a)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Renderer::parseColor(const std::string &colorName, int &r, int &g, int &b, int &a)
{
    a = 255; // Default alpha

    if (colorName == "blue")
    {
        r = 0;
        g = 128;
        b = 255;
    }
    else if (colorName == "yellow")
    {
        r = 255;
        g = 255;
        b = 0;
    }
    else if (colorName == "red")
    {
        r = 255;
        g = 0;
        b = 0;
    }
    else if (colorName == "green")
    {
        r = 0;
        g = 255;
        b = 0;
    }
    else if (colorName == "white")
    {
        r = 255;
        g = 255;
        b = 255;
    }
    else if (colorName == "black")
    {
        r = 0;
        g = 0;
        b = 0;
    }
    else
    {
        // Default to white for unknown colors
        r = 255;
        g = 255;
        b = 255;
    }
}
