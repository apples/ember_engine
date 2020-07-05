#include "math.hpp"

#include <sol.hpp>
#include <glm/glm.hpp>

namespace ember::math {

template <typename T>
using rel = T(&)(const T&, const T&);

template <typename T, typename U>
using rel_s = T(&)(const T&, U);

template <typename T, typename U>
using rel_m = U(&)(const T&, const U&);

template <typename T, typename U>
using rel_r = U(&)(const T&, const T&);

void register_types(sol::table& lua) {
    auto vec2_type = lua.new_usertype<glm::vec2>(
        "vec2", sol::constructors<glm::vec2(), glm::vec2(float, float), glm::vec2(const glm::vec2&)>{});
    vec2_type["x"] = &glm::vec2::x;
    vec2_type["y"] = &glm::vec2::y;
    vec2_type[sol::meta_function::index] = [](glm::vec2& v, int i) -> float { return v[i]; };
    vec2_type[sol::meta_function::new_index] = [](glm::vec2& v, int i, float x) { v[i] = x; };
    vec2_type[sol::meta_function::addition] = static_cast<rel<glm::vec2>>(glm::operator+);
    vec2_type[sol::meta_function::subtraction] = static_cast<rel<glm::vec2>>(glm::operator-);
    vec2_type[sol::meta_function::multiplication] = static_cast<rel_s<glm::vec2, float>>(glm::operator*);
    vec2_type["dot"] = static_cast<rel_r<glm::vec2, float>>(glm::dot);

    auto vec3_type = lua.new_usertype<glm::vec3>(
        "vec3",
        sol::constructors<
            glm::vec3(),
            glm::vec3(float, float, float),
            glm::vec3(const glm::vec2&, float),
            glm::vec3(const glm::vec3&)>{});
    vec3_type["x"] = &glm::vec3::x;
    vec3_type["y"] = &glm::vec3::y;
    vec3_type["z"] = &glm::vec3::z;
    vec3_type[sol::meta_function::index] = [](glm::vec3& v, int i) -> float { return v[i]; };
    vec3_type[sol::meta_function::new_index] = [](glm::vec3& v, int i, float x) { v[i] = x; };
    vec3_type[sol::meta_function::addition] = static_cast<rel<glm::vec3>>(glm::operator+);
    vec3_type[sol::meta_function::subtraction] = static_cast<rel<glm::vec3>>(glm::operator-);
    vec3_type[sol::meta_function::multiplication] = static_cast<rel_s<glm::vec3, float>>(glm::operator*);
    vec3_type["dot"] = static_cast<rel_r<glm::vec3, float>>(glm::dot);
    vec3_type["cross"] = static_cast<rel<glm::vec3>>(glm::cross);

    auto vec4_type = lua.new_usertype<glm::vec4>(
        "vec4",
        sol::constructors<
            glm::vec4(),
            glm::vec4(float, float, float, float),
            glm::vec4(const glm::vec2&, const glm::vec2&),
            glm::vec4(const glm::vec2&, float, float),
            glm ::vec4(const glm::vec3&, float),
            glm::vec4(const glm::vec4&)>{});
    vec4_type["x"] = &glm::vec4::x;
    vec4_type["y"] = &glm::vec4::y;
    vec4_type["z"] = &glm::vec4::z;
    vec4_type["w"] = &glm::vec4::w;
    vec4_type[sol::meta_function::index] = [](glm::vec4& v, int i) -> float { return v[i]; };
    vec4_type[sol::meta_function::new_index] = [](glm::vec4& v, int i, float x) { v[i] = x; };
    vec4_type[sol::meta_function::addition] = static_cast<rel<glm::vec4>>(glm::operator+);
    vec4_type[sol::meta_function::subtraction] = static_cast<rel<glm::vec4>>(glm::operator-);
    vec4_type[sol::meta_function::multiplication] = static_cast<rel_s<glm::vec4, const float&>>(glm::operator*);
    vec4_type["dot"] = static_cast<rel_r<glm::vec4, float>>(glm::dot);

    auto mat2_type = lua.new_usertype<glm::mat2>(
        "mat2", sol::constructors<glm::mat2(), glm::mat2(float), glm::mat2(const glm::mat2&)>{});
    mat2_type[sol::meta_function::index] = [](glm::mat2& m, int i) -> glm::vec2& { return m[i]; };
    mat2_type[sol::meta_function::new_index] = [](glm::mat2& m, int i, const glm::vec2& v) { m[i] = v; };
    mat2_type[sol::meta_function::multiplication] = sol::overload(
        static_cast<rel<glm::mat2>>(glm::operator*),
        static_cast<rel_m<glm::mat2, glm::vec2>>(glm::operator*));

    auto mat3_type = lua.new_usertype<glm::mat3>(
        "mat3", sol::constructors<glm::mat3(), glm::mat3(float), glm::mat3(const glm::mat3&)>{});
    mat3_type[sol::meta_function::index] = [](glm::mat3& m, int i) -> glm::vec3& { return m[i]; };
    mat3_type[sol::meta_function::new_index] = [](glm::mat3& m, int i, const glm::vec3& v) { m[i] = v; };
    mat3_type[sol::meta_function::multiplication] = sol::overload(
        static_cast<rel<glm::mat3>>(glm::operator*),
        static_cast<rel_m<glm::mat3, glm::vec3>>(glm::operator*));

    auto mat4_type = lua.new_usertype<glm::mat4>(
        "mat4", sol::constructors<glm::mat4(), glm::mat4(float), glm::mat4(const glm::mat4&)>{});
    mat4_type[sol::meta_function::index] = [](glm::mat4& m, int i) -> glm::vec4& { return m[i]; };
    mat4_type[sol::meta_function::new_index] = [](glm::mat4& m, int i, const glm::vec4& v) { m[i] = v; };
    mat4_type[sol::meta_function::multiplication] = sol::overload(
        static_cast<rel<glm::mat4>>(glm::operator*),
        static_cast<rel_m<glm::mat4, glm::vec4>>(glm::operator*));
}

} // namespace ember::math
