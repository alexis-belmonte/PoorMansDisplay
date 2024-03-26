#include "pmd/Color.hpp"

#include <algorithm>

namespace PMD
{
    Color Color::blend(Color a, Color b, double alpha)
    {
        return Color(
            static_cast<::uint8_t>(
                std::clamp((1.0 * a.c.r * a.c.a + alpha * b.c.r * b.c.a) / (1.0 * a.c.a + alpha * b.c.a),
                           0.0, 255.0)
            ),
            static_cast<::uint8_t>(
                std::clamp((1.0 * a.c.g * a.c.a + alpha * b.c.g * b.c.a) / (1.0 * a.c.a + alpha * b.c.a),
                           0.0, 255.0)
            ),
            static_cast<::uint8_t>(
                std::clamp((1.0 * a.c.b * a.c.a + alpha * b.c.b * b.c.a) / (1.0 * a.c.a + alpha * b.c.a),
                           0.0, 255.0)
            ),
            static_cast<::uint8_t>(
                std::clamp(1.0 * a.c.a + alpha * b.c.a, 0.0, 255.0)
            )
        );
    }

    Color Color::lerp(Color a, Color b, double t)
    {
        return Color(
            static_cast<::uint8_t>(std::clamp(a.c.r * (1.0 - t) + b.c.r * t, 0.0, 255.0)),
            static_cast<::uint8_t>(std::clamp(a.c.g * (1.0 - t) + b.c.g * t, 0.0, 255.0)),
            static_cast<::uint8_t>(std::clamp(a.c.b * (1.0 - t) + b.c.b * t, 0.0, 255.0)),
            static_cast<::uint8_t>(std::clamp(a.c.a * (1.0 - t) + b.c.a * t, 0.0, 255.0))
        );
    }
};
