#include "pmd/Color.hpp"

#include <algorithm>

namespace PMD
{
    Color Color::lerp(Color a, Color b, double t, bool additive)
    {
        return Color(
            static_cast<uint8_t>(a.c.r * (1 - t) + b.c.r * t),
            static_cast<uint8_t>(a.c.g * (1 - t) + b.c.g * t),
            static_cast<uint8_t>(a.c.b * (1 - t) + b.c.b * t),
                additive
                    ? static_cast<uint8_t>(std::min(a.c.a + b.c.a, 255))
                    : static_cast<uint8_t>(a.c.a * (1 - t) + b.c.a * t)
        );
    }
};
