#pragma once

#include <bit>

#include <cstdint>

namespace PMD
{
    union Color {
        /* Components */
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        } __attribute__((packed)) c;
        /* Value */
        uint32_t v;

        Color() :
            v(0)
        {}

        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff) :
            c{r, g, b, a}
        {}

        Color(uint32_t color)
        {
            this->c = {
                static_cast<uint8_t>((color >>  0) & 0xff),
                static_cast<uint8_t>((color >>  8) & 0xff),
                static_cast<uint8_t>((color >> 16) & 0xff),
                static_cast<uint8_t>((color >> 24) & 0xff)
            };
        }

        static Color lerp(Color a, Color b, double t, bool additiveAlpha = false);
    } __attribute__((packed));
};
