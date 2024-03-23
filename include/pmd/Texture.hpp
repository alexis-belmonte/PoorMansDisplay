#pragma once

#include "pmd/Vector.hpp"
#include "pmd/Rectangle.hpp"

#include "pmd/Color.hpp"

#include <functional>
#include <memory>
#include <limits>

#include <cstdint>

namespace PMD
{
    class Texture
    {
    public:
        enum class Filtering
        {
            NEAREST,
            LINEAR
        };

        Texture(Vector2u size);
        Texture(::size_t width, ::size_t height);

        Vector2u getSize() const;
        void resize(Vector2u size);

        Texture::Filtering getFiltering() const;
        void setFiltering(Texture::Filtering filtering);

        void access(std::function<void(Color *)> &&callback);
        void access(std::function<void(const Color *)> &&callback) const;

        void clear(Color color = 0x00000000);
        void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
        {
            this->clear(Color(r, g, b, a));
        }

        void blit(const Texture &source, Vector2f position, Vector2f scale = {1.0, 1.0}, Rectangle2u shearRect = {0u, 0u, ~0u, ~0u}, bool mapAlpha = false);
        void copy(const Texture &source, Vector2f position, Vector2f scale = {1.0, 1.0}, Rectangle2u shearRect = {0u, 0u, ~0u, ~0u})
        {
            this->blit(source, position, scale, shearRect, true);
        }

        void negate();

        // TODO: Add line/rectangle/circle/polygon drawing methods

    protected:
        Vector2u _size;
        std::unique_ptr<Color[]> _contents;

        Texture::Filtering _filtering = Texture::Filtering::NEAREST;
    };
};
