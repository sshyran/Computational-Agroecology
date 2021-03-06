#include "config.h"

namespace config {

Config::Config(const std::string &name, const Location &location)
    : name(name), location(location) {}

bool operator==(const Config &lhs, const Config &rhs) {
  return (lhs.location == rhs.location) && (lhs.name == rhs.name);
}

}  // namespace config