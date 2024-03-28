#include "pmd/Color.hpp"

#include <algorithm>

namespace PMD
{
    Color Color::blend(Color a, Color b, double alpha)
    {
        alpha = std::clamp(alpha, 0.0, 1.0);

        double rAlpha = a.c.a / 255.0 + alpha * (1.0 - a.c.a / 255.0);

        // Let's not assume rAlpha can never be zero
        if (rAlpha < 1e-6)
            return Color(0, 0, 0, 0);

        return Color(
            static_cast<::uint8_t>(
                std::clamp((a.c.r * a.c.a / 255.0 + alpha * b.c.r * b.c.a / 255.0) / rAlpha, 0.0, 255.0)
            ),
            static_cast<::uint8_t>(
                std::clamp((a.c.g * a.c.a / 255.0 + alpha * b.c.g * b.c.a / 255.0) / rAlpha, 0.0, 255.0)
            ),
            static_cast<::uint8_t>(
                std::clamp((a.c.b * a.c.a / 255.0 + alpha * b.c.b * b.c.a / 255.0) / rAlpha, 0.0, 255.0)
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
