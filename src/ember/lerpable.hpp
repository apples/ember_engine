#pragma once

namespace ember {

template <typename T = float>
class lerpable {
public:
    lerpable() = default;

    explicit lerpable(const T& t) : current_value(t), target_value(t), rem_time(0) {}

    void set_target(const T& t, float dur) {
        target_value = t;
        rem_time = dur;
    }

    void add_target(const T& t, float dur) {
        target_value += t;
        rem_time += dur;
    }

    void step(float delta) {
        if (delta < rem_time) {
            current_value += (target_value - current_value) * (delta / rem_time);
            rem_time -= delta;
        } else {
            current_value = target_value;
            rem_time = 0.f;
        }
    }

    const T& get_current() const { return current_value; }

    const T& get_target() const { return target_value; }

private:
    T current_value = static_cast<T>(0);
    T target_value = static_cast<T>(0);
    float rem_time = 0.f;
};

} // namespace ember
