#include "pmd/Texture.hpp"

namespace PMD
{
    Texture::Texture(const Vector2u &size) :
        _size(size),
        _contents(std::make_unique_for_overwrite<Color[]>(PMD::x(size) * PMD::y(size)))
    {}

    Texture::Texture(::size_t width, ::size_t height) :
        Texture(Vector2u{width, height})
    {}
    
    const Vector2u &Texture::getSize() const
    {
        return this->_size;
    }

    void Texture::resize(const Vector2u &size)
    {
        if (size == this->_size)
            return;

        this->_size = size;

        std::unique_ptr<Color[]> newContents(
            std::make_unique_for_overwrite<Color[]>(PMD::x(size) * PMD::y(size))
        );

        this->access([this, newContents = newContents.get()](const Color *contents) {
            for (::size_t i = 0; i < PMD::x(this->_size) * PMD::y(this->_size); i++)
                newContents[i] = contents[i];
        });

        this->_contents = std::move(newContents);
    }

    void Texture::access(std::function<void(Color *)> &&callback)
    {
        callback(this->_contents.get());
    }
    
    void Texture::access(std::function<void(const Color *)> &&callback) const
    {
        callback(this->_contents.get());
    }

    void Texture::clear(Color color)
    {
        this->access([this, color](Color *contents) {
            Vector2u size = this->getSize();

            for (Color *it = contents; it < contents + PMD::x(size) * PMD::y(size); ++it)
                *it = color;
        });
    }

    void Texture::copy(const Texture &source, const Vector2u &position)
    {
        this->access([this, position, &source](Color *targetContents) {
            Vector2u targetSize = this->getSize();
            Vector2u sourceSize = source.getSize();

            source.access([position, &targetSize, &sourceSize, targetContents](const Color *sourceContents) {
                for (::size_t y = 0; y < PMD::y(sourceSize) && PMD::y(position) + y < PMD::y(targetSize); y++)
                    for (::size_t x = 0; x < PMD::x(sourceSize) && PMD::x(position) + x < PMD::x(targetSize); x++) {
                        size_t sourceI =
                            y * PMD::x(sourceSize) + x;
                        size_t targetI =
                            (PMD::x(position) + x) +
                            (PMD::x(targetSize) * (PMD::y(position) + y));

                        targetContents[targetI] = sourceContents[sourceI];
                    }
            });
        });
    }

    void Texture::blit(const Texture &source, const Vector2u &position)
    {
        this->access([this, position, &source](Color *targetContents) {
            Vector2u targetSize = this->getSize();
            Vector2u sourceSize = source.getSize();

            source.access([position, &targetSize, &sourceSize, targetContents](const Color *sourceContents) {
                for (::size_t y = 0; y < PMD::y(sourceSize) && PMD::y(position) + y < PMD::y(targetSize); y++)
                    for (::size_t x = 0; x < PMD::x(sourceSize) && PMD::x(position) + x < PMD::x(targetSize); x++) {
                        size_t sourceI =
                            y * PMD::x(sourceSize) + x;
                        size_t targetI =
                            (PMD::x(position) + x) +
                            (PMD::x(targetSize) * (PMD::y(position) + y));
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
            for (::size_t i = 0; i < PMD::x(size) * PMD::y(size); i++)
                contents[i].v = ~contents[i].v;
        });
    }
};
