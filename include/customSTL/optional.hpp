#ifndef CUSTOM_STL_OPTIONAL_HPP
#define CUSTOM_STL_OPTIONAL_HPP

#include <optional>

namespace CustomSTL {

template <typename T>
class optional {
public:
    using value_type = T;

    constexpr optional() noexcept
        : val_ { nullptr }
        , is_valid_ { false }
    { }

    constexpr optional(std::nullopt_t)
        : optional()
    { }

    constexpr optional(const optional& other)
        : val_ {}

private:
    value_type val_;
    bool is_valid_;

};

} // namespace CustomSTL

#endif