#include "pmd/Color.hpp"

#include <algorithm>

namespace PMD
{
    Color Color::lerp(Color a, Color b, double k)
    {
        k = std::clamp(k, 0.0, 1.0);

        b.c.a *= k;

        double bAlpha = b.c.a / 255.0;
        double factor = a.c.a * (1.0 - bAlpha) / 255.0;

        return {
            static_cast<::uint8_t>(a.c.r * factor + b.c.r * bAlpha),
            static_cast<::uint8_t>(a.c.g * factor + b.c.g * bAlpha),
            static_cast<::uint8_t>(a.c.b * factor + b.c.b * bAlpha),
            static_cast<::uint8_t>(factor * 255.0 + b.c.a)
        };
    }
};
