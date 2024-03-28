#include "pmd/Color.hpp"

#include <algorithm>

namespace PMD
{
    Color Color::blend(Color a, Color b, double alpha)
    {
        alpha = std::clamp(alpha, 0.0, 1.0);

        double aAlpha = a.c.a / 255.0;
        double bAlpha = b.c.a / 255.0;

        double rAlpha = aAlpha + alpha * (1.0 - aAlpha);

        // Let's not assume rAlpha can never be zero
        if (rAlpha < 1e-6)
            return Color(0, 0, 0, 0);

        return Color(
            static_cast<::uint8_t>(
                std::clamp((a.c.r * aAlpha + alpha * b.c.r * bAlpha) / rAlpha, 0.0, 255.0)
            ),
            static_cast<::uint8_t>(
                std::clamp((a.c.g * aAlpha + alpha * b.c.g * bAlpha) / rAlpha, 0.0, 255.0)
            ),
            static_cast<::uint8_t>(
                std::clamp((a.c.b * aAlpha + alpha * b.c.b * bAlpha) / rAlpha, 0.0, 255.0)
            ),
            static_cast<::uint8_t>(
                rAlpha * 255.0
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
