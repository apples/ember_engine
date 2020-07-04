#pragma once

#include <sol.hpp>

#include <cmath>

namespace ember::math {

void register_types(sol::table& lua);

inline auto imod(double x, double y) -> int {
    return x - y * std::floor(x / y);
}

} // namespace ember::math
