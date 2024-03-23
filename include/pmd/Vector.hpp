#pragma once

#include <tuple>

#include <cstdint>

namespace PMD
{
    template <typename T>
    using Vector2  = std::tuple<T, T>;

    using Vector2u = Vector2<::uint32_t>;
    using Vector2i = Vector2<::int32_t>;
    using Vector2f = Vector2<double>;

    template <typename T>
    inline T x(const Vector2<T> &tuple)
    {
        return std::get<0>(tuple);
    }

    template <typename T>
    inline T y(const Vector2<T> &tuple)
    {
        return std::get<1>(tuple);
    }
};
