#pragma once
#include "Component.hpp"
#include "Entity.hpp"
#include "System.hpp"
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <iostream>

/**
 * @brief ECS Manager: Handles entities, components, and systems.
 */
class Manager
{
public:
  Manager();
  ~Manager();
  Entity createEntity();
  void destroyEntity(Entity entity);
  void removeEntity(Entity entity);
  std::vector<Entity> getAllEntities() const;

  // Component and system management methods to be implemented
private:
  Entity nextEntityId = 1;
  std::vector<Entity> entities;
  // Storage for components and systems
};

// --- ECS Component Storage (Header-only for templates) ---
#include <memory>
#include <typeindex>
#include <unordered_map>

// Forward declaration
std::unordered_map<std::type_index, std::unordered_map<Entity, std::shared_ptr<void>>> &getComponentStores();

/**
 * @brief Add a component to an entity.
 */
template <typename T>
void addComponent(Entity entity, const T &component)
{
  getComponentStores()[typeid(T)][entity] = std::make_shared<T>(component);
}

/**
 * @brief Get a pointer to a component of an entity. Returns nullptr if not
 * found.
 */
template <typename T>
T *getComponent(Entity entity)
{
  auto &componentStores = getComponentStores();
  auto typeIt = componentStores.find(typeid(T));
  if (typeIt == componentStores.end())
  {
    return nullptr;
  }

  auto it = typeIt->second.find(entity);
  if (it != typeIt->second.end())
  {
    return static_cast<T *>(it->second.get());
  }
  return nullptr;
}