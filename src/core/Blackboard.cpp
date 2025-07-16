#include "Blackboard.hpp"
#include <iostream>

void Blackboard::set(const std::string &key, const std::any &value)
{
  data[key] = value;
  std::cout << "[Blackboard] Set key: " << key << std::endl;
}

std::any Blackboard::get(const std::string &key) const
{
  auto it = data.find(key);
  if (it != data.end())
  {
    return it->second;
  }
  return std::any{}; // Return empty any if key not found
}

bool Blackboard::has(const std::string &key) const
{
  return data.find(key) != data.end();
}

void Blackboard::clear()
{
  data.clear();
  std::cout << "[Blackboard] Cleared all data" << std::endl;
}

void Blackboard::remove(const std::string &key)
{
  auto it = data.find(key);
  if (it != data.end())
  {
    data.erase(it);
    std::cout << "[Blackboard] Removed key: " << key << std::endl;
  }
}
