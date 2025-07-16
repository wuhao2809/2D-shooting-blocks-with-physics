#pragma once
#include <string>

/**
 * @brief Position component for 2D coordinates.
 */
struct Position
{
  float x, y;
};

/**
 * @brief Input component for controllable entities.
 */
struct Input
{
  bool controllable = false;
};

/**
 * @brief Renderable component for drawing entities.
 */
struct Renderable
{
  std::string color;
  int width, height;
  bool showDirection = false; // Show directional line
};

/**
 * @brief Direction component for entities that have orientation.
 */
struct Direction
{
  float angle = 0.0f; // Angle in degrees (0 = right, 90 = down, 180 = left, 270 = up)
};

/**
 * @brief Shooter component for entities that can shoot.
 */
struct Shooter
{
  float fireRate = 0.5f; // Bullets per second
  float lastShotTime = 0.0f;
  bool canShoot = true;
};

/**
 * @brief Bullet component for projectiles.
 */
struct Bullet
{
  float speed = 400.0f;  // pixels per second
  float lifetime = 3.0f; // seconds
  float timeAlive = 0.0f;
};

/**
 * @brief Velocity component for moving entities.
 */
struct Velocity
{
  float x = 0.0f;
  float y = 0.0f;
};