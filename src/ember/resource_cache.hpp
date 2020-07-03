#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <type_traits>
#include <map>

namespace ember {

/** Handles automatic caching of resources based on a key. */
template <typename T, typename... S>
class resource_cache {
public:
    using factory_function = std::function<std::shared_ptr<T>(const S&...)>;

    template <typename F>
    using require_is_factory = std::enable_if_t<std::is_convertible<std::decay_t<F>, factory_function>::value>*;

    template <typename F>
    using require_is_not_factory = std::enable_if_t<!std::is_convertible<std::decay_t<F>, factory_function>::value>*;

    resource_cache() = default;
    resource_cache(const resource_cache&) = delete;
    resource_cache(resource_cache&&) = default;
    resource_cache& operator=(const resource_cache&) = delete;
    resource_cache& operator=(resource_cache&&) = default;

    template <typename F>
    resource_cache(F&& f, require_is_factory<F> = {}) : factory(std::move(f)) {}

    template <typename F>
    resource_cache(F&& f, require_is_not_factory<F> = {}) : factory(make_factory(std::forward<F>(f))) {}

    std::shared_ptr<T> get(const S&... s) {
        auto key = std::tie(s...);
        auto iter = cache.find(key);

        if (iter != cache.end()) {
            return iter->second;
        } else {
            auto ptr = factory(s...);
            cache.insert_or_assign(key, ptr);
            return ptr;
        }
    }

    void clear() {
        cache.clear();
    }

    std::shared_ptr<T> reload(const S&... s) {
        auto& ptr = cache[std::tie(s...)];
        ptr = factory(s...);
        return ptr;
    }

private:
    template <typename F>
    static factory_function make_factory(F&& f) {
        return [f=std::forward<F>(f)](const S&... s) {
            return std::make_shared<T>(f(s...));
        };
    }

    factory_function factory;
    std::map<std::tuple<S...>, std::shared_ptr<T>> cache;
};

} // namespace ember
