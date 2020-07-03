#pragma once

#include <common/json.hpp>

namespace emberjs {

    extern "C" {
        extern char* ember_config_get();
    }

    nlohmann::json get_config();

} //namespace emberjs
