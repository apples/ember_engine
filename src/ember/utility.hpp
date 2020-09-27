#pragma once

#include <iterator>
#include <utility>

#define EMBER_CAT_IMPL(A,B) A##B
#define EMBER_CAT(A,B) EMBER_CAT_IMPL(A,B)
#define EMBER_DEFER auto EMBER_CAT(_defer_, __LINE__) = ::ember::utility::_defer{} + [&]()->void

namespace ember::utility {

template <typename T>
struct type_tag {
    using type = T;
};

template <typename... Ts>
struct reducer;

template <typename T, typename... Ts>
struct reducer<T, Ts...> {
    template <typename F, typename A>
    auto operator()(F&& func, A&& acc) const {
        return reducer<Ts...>{}(func, func(type_tag<T>{}, std::forward<A>(acc)));
    }
};

template <>
struct reducer<> {
    template <typename F, typename A>
    auto operator()(F&& func, A&& acc) const {
        return acc;
    }
};

template <typename... Ts>
struct type_list {
    template <typename F>
    static void apply(F&& func) {
        (func(type_tag<Ts>{}), ...);
    }

    template <typename F, typename T>
    static auto reduce(F&& func, T&& init) {
        return reducer<Ts...>{}(func, init);
    }
};

template <typename T>
struct type_list_from_variant;

template <template <typename...> class variant, typename... Ts>
struct type_list_from_variant<variant<Ts...>> {
    using type = type_list<Ts...>;
};

template <typename T>
using type_list_from_variant_t = typename type_list_from_variant<T>::type;

template <typename T, typename U>
struct variant_sum;

template <template <typename...> class variant, typename... Ts, typename... Us>
struct variant_sum<variant<Ts...>, variant<Us...>> {
    using type = variant<Ts..., Us...>;
};

template <template <typename...> class variant, typename T, typename... Us>
struct variant_sum<T, variant<Us...>> {
    using type = variant<T, Us...>;
};

template <typename T, typename U>
using variant_sum_t = typename variant_sum<T, U>::type;

struct _defer {
    template <typename F>
    struct _defer_impl {
        F func;
        bool active;
        _defer_impl(F f) : func(std::move(f)), active(true) {}
        _defer_impl(_defer_impl&& other) : func(std::move(other.func)), active(std::exchange(other.active, false)) {}
        ~_defer_impl() noexcept(false) {
            if (active)
                func();
        }
    };
    template <typename F>
    _defer_impl<F> operator+(F f) {
        return {std::move(f)};
    }
};

template <class... Fs>
struct overload : Fs... {
    overload(const Fs&... fs) : Fs{fs}... {}
    using Fs::operator()...;
};

template <typename T>
class reversed_impl {
public:
    reversed_impl(T t) : c(std::forward<T>(t)) {}

    auto begin() { return std::rbegin(c); }
    auto end() { return std::rend(c); }
    auto rbegin() { return std::begin(c); }
    auto rend() { return std::end(c); }

    auto begin() const { return std::rbegin(c); }
    auto end() const { return std::rend(c); }
    auto rbegin() const { return std::begin(c); }
    auto rend() const { return std::end(c); }

private:
    T c;
};

template <typename T>
auto reversed(T&& t) {
    return reversed_impl<T>(std::forward<T>(t));
}

template <typename T>
class span {
public:
    constexpr span() = default;

    constexpr span(T* d, std::size_t n): elements(d), length(n) {}

    constexpr T* begin() const { return elements; }

    constexpr T* end() const { return elements + length; }

    constexpr T* data() const { return elements; }

    constexpr std::size_t size() const { return length; }

    constexpr T& operator[](std::size_t i) const { return elements[i]; }

private:
    T* elements = nullptr;
    std::size_t length = 0;
};

} // namespace ember::utility
