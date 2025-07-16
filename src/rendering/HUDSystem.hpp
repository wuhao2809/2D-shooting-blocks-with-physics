#pragma once
#include "../core/System.hpp"
#include "../core/Components.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <chrono>

/**
 * @brief HUD system for displaying game information like FPS
 */
class HUDSystem : public System
{
public:
    HUDSystem(SDL_Renderer *renderer);
    ~HUDSystem();

    void update(float dt) override;
    void render();
    void toggleVisibility();
    void setVisible(bool visible);
    bool isVisible() const;

private:
    SDL_Renderer *renderer;
    bool hudVisible;

    // FPS tracking
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    float currentFPS;
    float frameTimeAccumulator;
    int frameCount;

    // Text rendering (simple bitmap font approach)
    void renderText(const std::string &text, int x, int y, SDL_Color color);
    void renderFPS();
    void updateFPS(float dt);

    // Simple bitmap font character rendering
    void renderCharacter(char c, int x, int y, SDL_Color color);
    void renderDigit(int digit, int x, int y, SDL_Color color);

    // Constants
    static constexpr int CHAR_WIDTH = 12;
    static constexpr int CHAR_HEIGHT = 16;
    static constexpr int HUD_MARGIN = 10;
    static constexpr float FPS_UPDATE_INTERVAL = 0.25f; // Update FPS display 4 times per second
};
