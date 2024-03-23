#include "pmd/Texture.hpp"

#include <cmath>

namespace PMD
{
    Texture::Texture(Vector2u size) :
        _size(size),
        _contents(std::make_unique_for_overwrite<Color[]>(PMD::x(size) * PMD::y(size)))
    {}

    Texture::Texture(::size_t width, ::size_t height) :
        Texture(Vector2u{width, height})
    {}
    
    Vector2u Texture::getSize() const
    {
        return this->_size;
    }

    void Texture::resize(Vector2u size)
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
    
    Texture::Filtering Texture::getFiltering() const
    {
        return this->_filtering;
    }

    void Texture::setFiltering(Texture::Filtering filtering)
    {
        this->_filtering = filtering;
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

    void Texture::copy(const Texture &source, Vector2u position, Vector2f scale, Rectangle2u shearRect)
    {
        Vector2u targetSize = this->getSize();
        Vector2u sourceSize = source.getSize();

        shearRect = {
            std::min(PMD::x(shearRect), PMD::x(sourceSize)),
            std::min(PMD::y(shearRect), PMD::y(sourceSize)),
            std::min(PMD::width(shearRect),  PMD::x(sourceSize) - PMD::x(shearRect)),
            std::min(PMD::height(shearRect), PMD::y(sourceSize) - PMD::y(shearRect))
        };

        this->access([=, this, &source](Color *targetContents) {
            source.access([=, this](const Color *sourceContents) {
                for (::size_t y = PMD::y(position);
                        y < PMD::y(position) + PMD::height(shearRect) * PMD::y(scale) &&
                            PMD::y(position) + y < PMD::y(targetSize);
                        y++)
                    for (::size_t x = PMD::x(position);
                            x < PMD::x(position) + PMD::width(shearRect) * PMD::y(scale) &&
                                PMD::x(position) + x < PMD::x(targetSize);
                            x++) {
                        Vector2f sourcePos{
                            PMD::x(shearRect) + (x - PMD::x(position)) / PMD::x(scale),
                            PMD::y(shearRect) + (y - PMD::y(position)) / PMD::y(scale)
                        };

                        Color sourceColor;
                        switch (this->_filtering) {
                        case Texture::Filtering::NEAREST:
                            sourceColor =
                                sourceContents[
                                    static_cast<::size_t>(std::round(PMD::y(sourcePos))) * PMD::x(sourceSize)
                                  + static_cast<::size_t>(std::round(PMD::x(sourcePos)))
                                ];
                            break;

                        case Texture::Filtering::LINEAR:
                            Color hColor = Color::lerp(
                                sourceContents[
                                    static_cast<::size_t>(std::floor(PMD::y(sourcePos))) * PMD::x(sourceSize)
                                  + static_cast<::size_t>(std::floor(PMD::x(sourcePos)))
                                ],
                                sourceContents[
                                    static_cast<::size_t>(std::floor(PMD::y(sourcePos))) * PMD::x(sourceSize)
                                  + static_cast<::size_t>(std::min(std::ceil(PMD::x(sourcePos)), PMD::x(sourceSize) - 1.0))
                                ],
                                PMD::x(sourcePos) - std::floor(PMD::x(sourcePos))
                            );

                            Color vColor = Color::lerp(
                                sourceContents[
                                    static_cast<::size_t>(std::min(std::ceil(PMD::y(sourcePos)), PMD::y(sourceSize) - 1.0)) * PMD::x(sourceSize)
                                  + static_cast<::size_t>(std::floor(PMD::x(sourcePos)))
                                ],
                                sourceContents[
                                    static_cast<::size_t>(std::min(std::ceil(PMD::y(sourcePos)), PMD::y(sourceSize) - 1.0)) * PMD::x(sourceSize)
                                  + static_cast<::size_t>(std::min(std::ceil(PMD::x(sourcePos)), PMD::x(sourceSize) - 1.0))
                                ],
                                PMD::x(sourcePos) - std::floor(PMD::x(sourcePos))
                            );

                            sourceColor = Color::lerp(
                                hColor,
                                vColor,
                                PMD::y(sourcePos) - std::floor(PMD::y(sourcePos))
                            );

                            break;
                        }
                        
                        targetContents[x + (PMD::x(targetSize) * y)] = sourceColor;
                    }
            });
        });
    }
    
    void Texture::blit(const Texture &source, Vector2f position, Vector2f scale, Rectangle2u shearRect)
    {
        this->access([this, position, &source](Color *targetContents) {
            Vector2u targetSize = this->getSize();
            Vector2u sourceSize = source.getSize();

            double fracX = PMD::x(position) - std::abs(PMD::x(position));
            double fracY = PMD::y(position) - std::abs(PMD::y(position));

            source.access([position, &targetSize, &sourceSize, targetContents, fracX, fracY](const Color *sourceContents) {
                for (::size_t y = 0; y < PMD::y(sourceSize) && PMD::y(position) + y < PMD::y(targetSize); y++)
                    for (::size_t x = 0; x < PMD::x(sourceSize) && PMD::x(position) + x < PMD::x(targetSize); x++) {
                        #define sourcePos(x, y) \
                            (y) * PMD::x(sourceSize) + (x)

                        #define targetPos(x, y) \
                            static_cast<::size_t>( \
                                (PMD::x(position) + (x)) + \
                                (PMD::x(targetSize) * (PMD::y(position) + (y))) \
                            )

                        double alpha =
                            sourceContents[sourcePos(x, y)].c.a / 255.0
                                * (1 - fracX) * (1 - fracY);

                        targetContents[targetPos(x, y)].c.r =
                            targetContents[targetPos(x, y)].c.r * (1 - alpha) + sourceContents[sourcePos(x, y)].c.r * alpha;
                        targetContents[targetPos(x, y)].c.g =
                            targetContents[targetPos(x, y)].c.g * (1 - alpha) + sourceContents[sourcePos(x, y)].c.g * alpha;
                        targetContents[targetPos(x, y)].c.b =
                            targetContents[targetPos(x, y)].c.b * (1 - alpha) + sourceContents[sourcePos(x, y)].c.b * alpha;
                        targetContents[targetPos(x, y)].c.a =
                            targetContents[targetPos(x, y)].c.a * (1 - alpha) + sourceContents[sourcePos(x, y)].c.a * alpha;
                        
                        #undef sourcePos
                        #undef targetPos
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
