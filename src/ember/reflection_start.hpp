
#define EMBER_REFLECTION_COMMA() ,
#define EMBER_REFLECTION_EMPTY
#define EMBER_REFLECTION_DEFER(...) __VA_ARGS__ EMBER_REFLECTION_EMPTY
#define EMBER_REFLECTION_CAT_(a, b) a##b
#define EMBER_REFLECTION_CAT(a, b) EMBER_REFLECTION_CAT_(a, b)

#define EMBER_REFLECTION_MEMBER(field) \
    ::ember::reflection::member<&type::field> { #field }
#define EMBER_REFLECTION_MEMBERS_0(x) EMBER_REFLECTION_MEMBER(x) EMBER_REFLECTION_MEMBERS_1
#define EMBER_REFLECTION_MEMBERS_1(x) \
    EMBER_REFLECTION_DEFER(EMBER_REFLECTION_COMMA)() EMBER_REFLECTION_MEMBER(x) EMBER_REFLECTION_MEMBERS_2
#define EMBER_REFLECTION_MEMBERS_2(x) \
    EMBER_REFLECTION_DEFER(EMBER_REFLECTION_COMMA)() EMBER_REFLECTION_MEMBER(x) EMBER_REFLECTION_MEMBERS_1
#define EMBER_REFLECTION_MEMBERS_0_END
#define EMBER_REFLECTION_MEMBERS_1_END
#define EMBER_REFLECTION_MEMBERS_2_END
#define EMBER_REFLECTION_MEMBERS(fields) EMBER_REFLECTION_CAT(EMBER_REFLECTION_MEMBERS_0 fields, _END)

#ifdef EMBER_REFLECTION_ENABLE_REGISTRY
#define EMBER_REFLECTION_GET_REGISTERED_TYPES \
    decltype(_type_registry(std::declval<_type_registry_index<EMBER_REFLECTION_MAX_TYPES>>()))

#define EMBER_REFLECTION_REGISTER_TYPE(type)                                                             \
    inline decltype(std::tuple_cat(                                                                      \
        std::declval<EMBER_REFLECTION_GET_REGISTERED_TYPES>(),                                           \
        std::declval<std::tuple<::ember::reflection::tag<type>>>()))                                     \
    _type_registry(_type_registry_index<std::tuple_size_v<EMBER_REFLECTION_GET_REGISTERED_TYPES> + 1>) { \
        return {};                                                                                       \
    }
#else
#define EMBER_REFLECTION_GET_REGISTERED_TYPES
#define EMBER_REFLECTION_REGISTER_TYPE(type)
#endif

#define REFLECT(name, ...)                                                                                    \
    constexpr auto reflect(::ember::reflection::tag<name>) {                                                  \
        using type = name;                                                                                    \
        return ::ember::reflection::make_refl_info(                                                           \
            ::ember::reflection::tag<name>{}, #name, std::make_tuple(EMBER_REFLECTION_MEMBERS(__VA_ARGS__))); \
    }                                                                                                         \
    EMBER_REFLECTION_REGISTER_TYPE(name)
