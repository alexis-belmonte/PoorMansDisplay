#pragma once

#include <tuple>

#include <cstdint>

namespace PMD
{
    template<typename T>
    using Rectangle2 = std::tuple<T, T, T, T>;

    using Rectangle2u = Rectangle2<::uint32_t>;
    using Rectangle2i = Rectangle2<::int32_t>;
    using Rectangle2f = Rectangle2<double>;

    template<typename T>
    inline T x(const Rectangle2<T> &tuple)
    {
        return std::get<0>(tuple);
    }

    template<typename T>
    inline T y(const Rectangle2<T> &tuple)
    {
        return std::get<1>(tuple);
    }

    template<typename T>
    inline T width(const Rectangle2<T> &tuple)
    {
        return std::get<2>(tuple);
    }

    template<typename T>
    inline T height(const Rectangle2<T> &tuple)
    {
        return std::get<3>(tuple);
    }
};
