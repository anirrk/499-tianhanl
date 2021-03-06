#include "store.h"

namespace chirpsystem {
std::optional<std::string> Store::Get(const std::string &key) {
  std::lock_guard<std::mutex> guard(map_mutex_);
  auto search_result = map_.find(key);
  if (search_result == map_.end()) {
    return std::nullopt;
  }

  return std::optional<std::string>(search_result->second);
}

bool Store::Put(const std::string &key, const std::string &value) {
  std::lock_guard<std::mutex> guard(map_mutex_);
  map_[key] = value;
  return true;
}

// return false if key is not exist
bool Store::Remove(const std::string &key) {
  std::lock_guard<std::mutex> guard(map_mutex_);
  return map_.erase(key);
}
}  // namespace chirpsystem
