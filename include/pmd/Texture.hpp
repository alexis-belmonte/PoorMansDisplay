#pragma once

#include "pmd/Vector.hpp"
#include "pmd/Color.hpp"

#include <functional>

#include <cstdint>

namespace PMD
{
    class Texture
    {
    public:
        Texture(const Vector2u &size);

        const Vector2u &getSize() const;

        void access(std::function<void(::uint32_t *)> &&callback);

        void clear(Color color = 0x00000000);
        void blit(const Vector2u &position, const Texture &source);
        void negate();

    protected:
        Vector2u _size;
        ::uint32_t *_contents = nullptr;
    };
};
