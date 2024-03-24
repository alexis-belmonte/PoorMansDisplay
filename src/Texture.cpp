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

    void Texture::copy(const Texture &source, Vector2u position)
    {
        Vector2u targetSize = this->getSize();
        Vector2u sourceSize = source.getSize();

        this->access([=, &source](Color *targetContents) {
            source.access([=](const Color *sourceContents) {
                for (::size_t y = 0; y < PMD::y(sourceSize); y++)
                    for (::size_t x = 0; x < PMD::x(sourceSize); x++) {
                        if (PMD::x(position) + x >= PMD::x(targetSize) || PMD::y(position) + y >= PMD::y(targetSize))
                            continue;

                        targetContents[
                            (PMD::y(position) + y) * PMD::x(targetSize) + (PMD::x(position) + x)
                        ] = sourceContents[y * PMD::x(sourceSize) + x];
                    }
            });
        });
    }

    void Texture::clear(Color color)
    {
        this->access([this, color](Color *contents) {
            Vector2u size = this->getSize();

            for (Color *it = contents; it < contents + PMD::x(size) * PMD::y(size); ++it)
                *it = color;
        });
    }

    void Texture::blit(const Texture &source, Vector2f position, Vector2f scale, Rectangle2u shearRect)
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
                for (double y = PMD::y(position);
                        y < PMD::y(position) + PMD::height(shearRect) * PMD::y(scale) &&
                            y < PMD::y(targetSize);
                        y++)
                    for (double x = PMD::x(position);
                            x < PMD::x(position) + PMD::width(shearRect) * PMD::x(scale) &&
                                x < PMD::x(targetSize);
                            x++) {
                        Vector2f sourcePos{
                            PMD::x(shearRect) + (x - PMD::x(position)) / PMD::x(scale),
                            PMD::y(shearRect) + (y - PMD::y(position)) / PMD::y(scale)
                        };

                        Color sourceColor;
                        switch (this->_filtering) {
                            case Texture::Filtering::NEAREST: {
                                sourceColor =
                                    sourceContents[
                                        static_cast<::size_t>(std::round(PMD::y(sourcePos))) * PMD::x(sourceSize)
                                      + static_cast<::size_t>(std::round(PMD::x(sourcePos)))
                                    ];
                                break;
                            }

                            case Texture::Filtering::LINEAR: {
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
                        }

                        double sourceAlpha = sourceColor.c.a / 255.0;

                        // TODO: support negative scale 

                        switch (this->_filtering) {
                            case Texture::Filtering::NEAREST: {
                                Color *targetColor = &targetContents[static_cast<::size_t>(
                                    std::floor(x) + (PMD::x(targetSize) * std::floor(y))
                                )];

                                *targetColor = Color::lerp(*targetColor, sourceColor, sourceAlpha, true);
                                break;
                            }

                            case Texture::Filtering::LINEAR: {
                                Color *target00 = &targetContents[static_cast<::size_t>(
                                    std::floor(x) + (PMD::x(targetSize) * std::floor(y))
                                )];
                                Color *target01 = &targetContents[static_cast<::size_t>(
                                    std::floor(x) + (PMD::x(targetSize) * std::ceil(y))
                                )];
                                Color *target10 = &targetContents[static_cast<::size_t>(
                                    std::ceil(x) + (PMD::x(targetSize) * std::floor(y))
                                )];
                                Color *target11 = &targetContents[static_cast<::size_t>(
                                    std::ceil(x) + (PMD::x(targetSize) * std::ceil(y))
                                )];

                                double fracX = PMD::x(position) - std::floor(PMD::x(position));
                                double fracY = PMD::y(position) - std::floor(PMD::y(position));

                                *target00 = Color::lerp(*target00, sourceColor, sourceAlpha * (1.0 - fracX) * (1.0 - fracY), true);
                                *target01 = Color::lerp(*target01, sourceColor, sourceAlpha * (1.0 - fracX) *        fracY,  true);
                                *target10 = Color::lerp(*target10, sourceColor, sourceAlpha *        fracX  * (1.0 - fracY), true);
                                *target11 = Color::lerp(*target11, sourceColor, sourceAlpha *        fracX  *        fracY,  true);

                                break;
                            }
                        }
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
