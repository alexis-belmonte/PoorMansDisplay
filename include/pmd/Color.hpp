#pragma once

#include <cstdint>

namespace PMD
{
    using Color = ::uint32_t;

    inline Color fromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    inline Color fromRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        return fromRGBA(r, g, b, 0xff);
    }
};
