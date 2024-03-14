#include "pmd/Texture.hpp"

namespace PMD
{
    Texture::Texture(const Vector2u &size) :
        _size(size),
        _contents(new Color[std::get<0>(size) * std::get<1>(size)])
    {}

    Texture::Texture(::size_t width, ::size_t height) :
        Texture(Vector2u{width, height})
    {}
    
    const Vector2u &Texture::getSize() const
    {
        return this->_size;
    }

    void Texture::access(std::function<void(Color *)> &&callback)
    {
        callback(this->_contents);
    }
    
    void Texture::access(std::function<void(const Color *)> &&callback) const
    {
        callback(this->_contents);
    }

    void Texture::clear(Color color)
    {
        this->access([this, color](Color *contents) {
            Vector2u size = this->getSize();

            for (Color *it = contents; it < contents + std::get<0>(size) * std::get<1>(size); ++it)
                *it = color;
        });
    }

    void Texture::blit(const Vector2u &position, const Texture &source)
    {
        this->access([this, position, source](Color *targetContents) {
            Vector2u targetSize = this->getSize();
            Vector2u sourceSize = source.getSize();

            source.access([position, &targetSize, &sourceSize, targetContents](const Color *sourceContents) {
                for (::size_t y = 0; y < std::get<1>(sourceSize) && std::get<1>(position) + y < std::get<1>(targetSize); y++)
                    for (::size_t x = 0; x < std::get<0>(sourceSize) && std::get<0>(position) + x < std::get<0>(targetSize); x++) {
                        size_t sourceI =
                            y * std::get<0>(sourceSize) + x;
                        size_t targetI =
                            (std::get<0>(position) + x) +
                            (std::get<0>(targetSize) * (std::get<1>(position) + y));
                        double alpha = sourceContents[sourceI].c.a / 255.0;

                        targetContents[targetI].c.r = targetContents[targetI].c.r * (1 - alpha) + sourceContents[sourceI].c.r * alpha;
                        targetContents[targetI].c.g = targetContents[targetI].c.g * (1 - alpha) + sourceContents[sourceI].c.g * alpha;
                        targetContents[targetI].c.b = targetContents[targetI].c.b * (1 - alpha) + sourceContents[sourceI].c.b * alpha;
                    }
            });
        });
    }

    void Texture::negate()
    {
        this->access([this](Color *contents) {
            Vector2u size = this->getSize();
            for (::size_t i = 0; i < std::get<0>(size) * std::get<1>(size); i++)
                contents[i].v = ~contents[i].v;
        });
    }
};
