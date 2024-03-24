#include "pmd/Color.hpp"

#include <algorithm>

namespace PMD
{
    Color Color::lerp(Color a, Color b, double t, bool additive)
    {
        if (additive)
            return Color(
                static_cast<uint8_t>(std::clamp(a.c.r + b.c.r * t, 0.0, 255.0)),
                static_cast<uint8_t>(std::clamp(a.c.g + b.c.g * t, 0.0, 255.0)),
                static_cast<uint8_t>(std::clamp(a.c.b + b.c.b * t, 0.0, 255.0)),
                static_cast<uint8_t>(std::clamp(a.c.a + b.c.a * t, 0.0, 255.0))
            );
        else
            return Color(
                static_cast<uint8_t>(a.c.r * (1 - t) + b.c.r * t),
                static_cast<uint8_t>(a.c.g * (1 - t) + b.c.g * t),
                static_cast<uint8_t>(a.c.b * (1 - t) + b.c.b * t),
                static_cast<uint8_t>(a.c.a * (1 - t) + b.c.a * t)
            );
    }
};
