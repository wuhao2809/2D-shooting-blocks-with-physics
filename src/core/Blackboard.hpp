#pragma once
#include <any>
#include <string>
#include <unordered_map>

/**
 * @brief Blackboard system for inter-system communication.
 * @note Requires C++17 for std::any.
 */
class Blackboard
{
public:
  void set(const std::string &key, const std::any &value);
  std::any get(const std::string &key) const;
  bool has(const std::string &key) const;
  void clear();
  void remove(const std::string &key);

  // Template helper for type-safe access
  template <typename T>
  T getValue(const std::string &key) const
  {
    auto it = data.find(key);
    if (it != data.end())
    {
      try
      {
        return std::any_cast<T>(it->second);
      }
      catch (const std::bad_any_cast &e)
      {
        // Return default value if cast fails
        return T{};
      }
    }
    return T{};
  }

  // Template helper for type-safe setting
  template <typename T>
  void setValue(const std::string &key, const T &value)
  {
    set(key, std::any(value));
  }

private:
  std::unordered_map<std::string, std::any> data;
};