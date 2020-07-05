#pragma once

#include "json.hpp"
#include "json_serializers.hpp"

#include <string>

#include "reflection_start.hpp"

namespace ember::config {

using json_serializers::basic::from_json;
using json_serializers::basic::to_json;

struct display_t {
    int width;
    int height;
};
REFLECT(display_t, (width)(height))

struct config {
    display_t display;
};
REFLECT(config, (display))

} // namespace ember::config

#include "reflection_end.hpp"
