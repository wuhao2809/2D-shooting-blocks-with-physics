#include "HUDSystem.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

HUDSystem::HUDSystem(SDL_Renderer *renderer)
    : renderer(renderer), hudVisible(true), currentFPS(60.0f),
      frameTimeAccumulator(0.0f), frameCount(0)
{
    lastFrameTime = std::chrono::high_resolution_clock::now();
    std::cout << "[HUDSystem] Initialized" << std::endl;
}

HUDSystem::~HUDSystem()
{
    std::cout << "[HUDSystem] Destroyed" << std::endl;
}

void HUDSystem::update(float dt)
{
    updateFPS(dt);

    // Check for HUD toggle request from blackboard
    if (blackboard && blackboard->has("hud_toggle_request") &&
        blackboard->getValue<bool>("hud_toggle_request"))
    {
        toggleVisibility();
        blackboard->setValue("hud_toggle_request", false);
    }
}

void HUDSystem::render()
{
    if (!hudVisible || !renderer)
        return;

    renderFPS();
}

void HUDSystem::toggleVisibility()
{
    hudVisible = !hudVisible;
    std::cout << "[HUDSystem] HUD visibility toggled: " << (hudVisible ? "ON" : "OFF") << std::endl;
}

void HUDSystem::setVisible(bool visible)
{
    hudVisible = visible;
}

bool HUDSystem::isVisible() const
{
    return hudVisible;
}

void HUDSystem::updateFPS(float dt)
{
    frameTimeAccumulator += dt;
    frameCount++;

    // Update FPS display every FPS_UPDATE_INTERVAL seconds
    if (frameTimeAccumulator >= FPS_UPDATE_INTERVAL)
    {
        currentFPS = frameCount / frameTimeAccumulator;
        frameTimeAccumulator = 0.0f;
        frameCount = 0;
    }
}

void HUDSystem::renderFPS()
{
    // Create FPS text
    std::stringstream ss;
    ss << "FPS: " << std::fixed << std::setprecision(1) << currentFPS;
    std::string fpsText = ss.str();

    // Render FPS text at top-left corner
    SDL_Color green = {0, 255, 0, 255};
    renderText(fpsText, HUD_MARGIN, HUD_MARGIN, green);

    // Render instructions
    renderText("H: Toggle HUD", HUD_MARGIN, HUD_MARGIN + CHAR_HEIGHT + 5, green);
    renderText("ESC: Exit Game", HUD_MARGIN, HUD_MARGIN + 2 * (CHAR_HEIGHT + 5), green);
}

void HUDSystem::renderText(const std::string &text, int x, int y, SDL_Color color)
{
    int currentX = x;

    for (char c : text)
    {
        if (c == ' ')
        {
            currentX += CHAR_WIDTH;
            continue;
        }

        renderCharacter(c, currentX, y, color);
        currentX += CHAR_WIDTH;
    }
}

void HUDSystem::renderCharacter(char c, int x, int y, SDL_Color color)
{
    // Set render color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Simple bitmap font rendering - just basic characters
    // Render lines multiple times to make them thicker
    auto renderThickLine = [&](int x1, int y1, int x2, int y2)
    {
        SDL_RenderLine(renderer, x1, y1, x2, y2);
        SDL_RenderLine(renderer, x1, y1 + 1, x2, y2 + 1);
        SDL_RenderLine(renderer, x1 + 1, y1, x2 + 1, y2);
    };

    switch (c)
    {
    case 'F':
        // F shape
        renderThickLine(x, y, x, y + CHAR_HEIGHT - 1);
        renderThickLine(x, y, x + CHAR_WIDTH - 2, y);
        renderThickLine(x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 3, y + CHAR_HEIGHT / 2);
        break;

    case 'P':
        // P shape
        renderThickLine(x, y, x, y + CHAR_HEIGHT - 1);
        renderThickLine(x, y, x + CHAR_WIDTH - 2, y);
        renderThickLine(x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        renderThickLine(x + CHAR_WIDTH - 2, y, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        break;

    case 'S':
        // S shape
        renderThickLine(x, y, x + CHAR_WIDTH - 1, y);
        renderThickLine(x, y, x, y + CHAR_HEIGHT / 2);
        renderThickLine(x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        renderThickLine(x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        renderThickLine(x, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        break;

    case 'H':
        // H shape
        SDL_RenderLine(renderer, x, y, x, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        break;

    case 'U':
        // U shape
        SDL_RenderLine(renderer, x, y, x, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        break;

    case 'D':
        // D shape
        SDL_RenderLine(renderer, x, y, x, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y, x + CHAR_WIDTH - 2, y);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 2);
        break;

    case 'T':
        // T shape
        SDL_RenderLine(renderer, x, y, x + CHAR_WIDTH - 1, y);
        SDL_RenderLine(renderer, x + CHAR_WIDTH / 2, y, x + CHAR_WIDTH / 2, y + CHAR_HEIGHT - 1);
        break;

    case 'o':
        // o shape (lowercase)
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2 + 1, x, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2 + 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 2);
        break;

    case 'g':
        // g shape (lowercase)
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT - 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2 + 1, x, y + CHAR_HEIGHT - 3);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2 + 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT + 1);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT);
        break;

    case 'l':
        // l shape (lowercase)
        SDL_RenderLine(renderer, x + CHAR_WIDTH / 2, y, x + CHAR_WIDTH / 2, y + CHAR_HEIGHT - 1);
        break;

    case 'e':
        // e shape (lowercase)
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2 + 1, x, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2 + 1, x + CHAR_WIDTH - 1, y + 3 * CHAR_HEIGHT / 4);
        SDL_RenderLine(renderer, x + 1, y + 3 * CHAR_HEIGHT / 4, x + CHAR_WIDTH - 2, y + 3 * CHAR_HEIGHT / 4);
        break;

    case 'x':
        // x shape (lowercase)
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2, x, y + CHAR_HEIGHT - 1);
        break;

    case 'i':
        // i shape (lowercase)
        SDL_RenderLine(renderer, x + CHAR_WIDTH / 2, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH / 2, y + CHAR_HEIGHT - 1);
        // Render a small dot for the i
        {
            SDL_FRect dotRect = {static_cast<float>(x + CHAR_WIDTH / 2), static_cast<float>(y + CHAR_HEIGHT / 2 - 2), 2.0f, 2.0f};
            SDL_RenderFillRect(renderer, &dotRect);
        }
        break;

    case 't':
        // t shape (lowercase)
        SDL_RenderLine(renderer, x + CHAR_WIDTH / 2, y + 1, x + CHAR_WIDTH / 2, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        break;

    case 'G':
        // G shape
        SDL_RenderLine(renderer, x + 1, y, x + CHAR_WIDTH - 1, y);
        SDL_RenderLine(renderer, x, y + 1, x, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + CHAR_WIDTH / 2, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        break;

    case 'a':
        // a shape (lowercase)
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x + 1, y + 3 * CHAR_HEIGHT / 4, x + CHAR_WIDTH - 2, y + 3 * CHAR_HEIGHT / 4);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2 + 1, x, y + 3 * CHAR_HEIGHT / 4 - 1);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2 + 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT - 1);
        break;

    case 'm':
        // m shape (lowercase)
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x + CHAR_WIDTH / 3, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH / 3, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x + 2 * CHAR_WIDTH / 3, y + CHAR_HEIGHT / 2, x + 2 * CHAR_WIDTH / 3, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        break;

    case 'E':
        // E shape
        SDL_RenderLine(renderer, x, y, x, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y, x + CHAR_WIDTH - 1, y);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        break;

    case 'C':
        // C shape
        SDL_RenderLine(renderer, x + 1, y, x + CHAR_WIDTH - 1, y);
        SDL_RenderLine(renderer, x, y + 1, x, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        break;

    case ':':
        // Colon
        {
            SDL_FRect dot1 = {static_cast<float>(x + CHAR_WIDTH / 2 - 1), static_cast<float>(y + CHAR_HEIGHT / 3), 3.0f, 3.0f};
            SDL_FRect dot2 = {static_cast<float>(x + CHAR_WIDTH / 2 - 1), static_cast<float>(y + 2 * CHAR_HEIGHT / 3), 3.0f, 3.0f};
            SDL_RenderFillRect(renderer, &dot1);
            SDL_RenderFillRect(renderer, &dot2);
        }
        break;

    default:
        // For digits, use the digit renderer
        if (c >= '0' && c <= '9')
        {
            renderDigit(c - '0', x, y, color);
        }
        else
        {
            // Unknown character - render as a small rectangle
            SDL_FRect rect = {static_cast<float>(x), static_cast<float>(y + CHAR_HEIGHT / 2), static_cast<float>(CHAR_WIDTH / 2), static_cast<float>(CHAR_HEIGHT / 4)};
            SDL_RenderFillRect(renderer, &rect);
        }
        break;
    }
}

void HUDSystem::renderDigit(int digit, int x, int y, SDL_Color color)
{
    // Set render color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Render lines multiple times to make them thicker
    auto renderThickLine = [&](int x1, int y1, int x2, int y2)
    {
        SDL_RenderLine(renderer, x1, y1, x2, y2);
        SDL_RenderLine(renderer, x1, y1 + 1, x2, y2 + 1);
        SDL_RenderLine(renderer, x1 + 1, y1, x2 + 1, y2);
    };

    switch (digit)
    {
    case 0:
        renderThickLine(x + 1, y, x + CHAR_WIDTH - 2, y);
        renderThickLine(x + 1, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT - 1);
        renderThickLine(x, y + 1, x, y + CHAR_HEIGHT - 2);
        renderThickLine(x + CHAR_WIDTH - 1, y + 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 2);
        break;

    case 1:
        SDL_RenderLine(renderer, x + CHAR_WIDTH / 2, y, x + CHAR_WIDTH / 2, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x + CHAR_WIDTH / 2 - 1, y + 1, x + CHAR_WIDTH / 2, y);
        break;

    case 2:
        SDL_RenderLine(renderer, x, y, x + CHAR_WIDTH - 1, y);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        break;

    case 3:
        SDL_RenderLine(renderer, x, y, x + CHAR_WIDTH - 1, y);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        break;

    case 4:
        SDL_RenderLine(renderer, x, y, x, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        break;

    case 5:
        SDL_RenderLine(renderer, x, y, x + CHAR_WIDTH - 1, y);
        SDL_RenderLine(renderer, x, y, x, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        SDL_RenderLine(renderer, x, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        break;

    case 6:
        SDL_RenderLine(renderer, x + 1, y, x + CHAR_WIDTH - 1, y);
        SDL_RenderLine(renderer, x, y + 1, x, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT / 2 + 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT - 1);
        break;

    case 7:
        SDL_RenderLine(renderer, x, y, x + CHAR_WIDTH - 1, y);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 1);
        break;

    case 8:
        SDL_RenderLine(renderer, x + 1, y, x + CHAR_WIDTH - 2, y);
        SDL_RenderLine(renderer, x, y + 1, x, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT - 1);
        break;

    case 9:
        SDL_RenderLine(renderer, x + 1, y, x + CHAR_WIDTH - 2, y);
        SDL_RenderLine(renderer, x, y + 1, x, y + CHAR_HEIGHT / 2 - 1);
        SDL_RenderLine(renderer, x + CHAR_WIDTH - 1, y + 1, x + CHAR_WIDTH - 1, y + CHAR_HEIGHT - 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT / 2, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT / 2);
        SDL_RenderLine(renderer, x + 1, y + CHAR_HEIGHT - 1, x + CHAR_WIDTH - 2, y + CHAR_HEIGHT - 1);
        break;
    }
}
