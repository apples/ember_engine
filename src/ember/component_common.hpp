#pragma once

#include "json_serializers.hpp"

#include <tuple>

#ifndef EMBER_COMPONENT_MAX_TYPES
#define EMBER_COMPONENT_MAX_TYPES 32
#endif

namespace component {

using json_serializers::basic::from_json;
using json_serializers::basic::to_json;

// Type registry
template <std::size_t N>
struct _type_registry_index : _type_registry_index<N - 1> {};
template <>
struct _type_registry_index<0> {};
inline std::tuple<> _type_registry(_type_registry_index<0>) { return {}; }
inline auto _type_registry(_type_registry_index<EMBER_COMPONENT_MAX_TYPES + 1>) = delete;

} // namespace component
