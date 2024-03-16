#pragma once

#include <tuple>

#include <cstdint>

namespace PMD
{
    using Vector2u = std::tuple<::uint32_t, ::uint32_t>;
    using Vector2i = std::tuple<::int32_t, ::int32_t>;

    template <typename T>
    inline T x(const std::tuple<T, T> &tuple)
    {
        return std::get<0>(tuple);
    }

    template <typename T>
    inline T y(const std::tuple<T, T> &tuple)
    {
        return std::get<1>(tuple);
    }
};
