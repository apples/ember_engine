#pragma once

#include <cstdint>

/** Canonical never-recycled ID for any entity, useful for serialization */
namespace ember {

struct net_id {
    using id_type = std::int64_t;
    id_type id; /** Canonical ID */
};

} // namespace ember
