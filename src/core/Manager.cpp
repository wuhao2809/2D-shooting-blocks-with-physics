#include "Manager.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <algorithm>
#include <iostream>

// Global component storage
std::unordered_map<std::type_index, std::unordered_map<Entity, std::shared_ptr<void>>> &getComponentStores()
{
  static std::unordered_map<std::type_index, std::unordered_map<Entity, std::shared_ptr<void>>> componentStores;
  return componentStores;
}

Manager::Manager() {}
Manager::~Manager() {}

Entity Manager::createEntity()
{
  Entity entity = nextEntityId++;
  entities.push_back(entity);
  std::cout << "[Manager] Created entity " << entity << std::endl;
  return entity;
}

void Manager::destroyEntity(Entity entity)
{
  // Not implemented for prototype
}

void Manager::removeEntity(Entity entity)
{
  // Remove from entities list
  entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());

  // Remove all components for this entity
  auto &componentStores = getComponentStores();
  for (auto &[typeIndex, entityMap] : componentStores)
  {
    entityMap.erase(entity);
  }

  std::cout << "[Manager] Removed entity " << entity << std::endl;
}

std::vector<Entity> Manager::getAllEntities() const
{
  return entities;
}