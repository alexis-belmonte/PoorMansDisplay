#pragma once

#include "pmd/Vector.hpp"
#include "pmd/Color.hpp"

#include <functional>
#include <memory>

#include <cstdint>

namespace PMD
{
    class Texture
    {
    public:
        Texture(const Vector2u &size);
        Texture(::size_t width, ::size_t height);

        const Vector2u &getSize() const;
        void resize(const Vector2u &size);

        void access(std::function<void(Color *)> &&callback);
        void access(std::function<void(const Color *)> &&callback) const;

        void clear(Color color = 0x00000000);
        void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
        {
            this->clear(Color(r, g, b, a));
        }

        void copy(const Texture &source, const Vector2u &position);
        void blit(const Texture &source, const Vector2u &position);

        void negate();

        // TODO: Add line/rectangle/circle/polygon drawing methods

    protected:
        Vector2u _size;
        std::unique_ptr<Color[]> _contents;
    };
};
