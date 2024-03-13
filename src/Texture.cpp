#include "pmd/Texture.hpp"

namespace PMD
{
    Texture::Texture(const Vector2u &size) :
        _size(size),
        _contents(new ::uint32_t[std::get<0>(size) * std::get<1>(size)])
    {
        this->clear();
    }
    
    const Vector2u &Texture::getSize() const
    {
        return this->_size;
    }

    void Texture::access(std::function<void(::uint32_t *)> &&callback)
    {
        callback(this->_contents);
    }

    void Texture::clear(Color color)
    {
        this->access([this, color](::uint32_t *contents) {
            Vector2u size = this->getSize();

            for (::uint32_t *it = contents; it < contents + std::get<0>(size) * std::get<1>(size); ++it)
                *it = color;
        });
    }

    void Texture::blit(const Vector2u &position, const Texture &source)
    {
        this->access([this, position, source](::uint32_t *contents) {
            Vector2u size = this->getSize();
            Vector2u sourceSize = source.getSize();

            for (::size_t y = 0; y < std::get<1>(sourceSize); y++)
                for (::size_t x = 0; x < std::get<0>(sourceSize); x++)
                    contents[(std::get<0>(position) + x) + (std::get<0>(size) * (std::get<1>(position) + y))] = source._contents[x + (std::get<0>(sourceSize) * y)];
        });
    }

    void Texture::negate()
    {
        this->access([this](::uint32_t *contents) {
            Vector2u size = this->getSize();
            for (::size_t i = 0; i < std::get<0>(size) * std::get<1>(size); i++)
                contents[i] = ~contents[i];
        });
    }
};
