#pragma once
#include <any>
#include <string>
#include <unordered_map>

/**
 * @brief Blackboard system for inter-system communication.
 * @note Requires C++17 for std::any.
 */
class Blackboard {
public:
  void set(const std::string &key, const std::any &value);
  std::any get(const std::string &key) const;
  bool has(const std::string &key) const;

private:
  std::unordered_map<std::string, std::any> data;
};