#include "pmd/Texture.hpp"

#include <cmath>
#include <numbers>

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

        std::unique_ptr<Color[]> newContents(
            std::make_unique_for_overwrite<Color[]>(PMD::x(size) * PMD::y(size))
        );

        this->access([this, newContents = newContents.get()](const Color *contents) {
            for (::size_t i = 0; i < PMD::x(this->_size) * PMD::y(this->_size); i++)
                newContents[i] = contents[i];
        });

        this->_size = size;
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

    void Texture::blit(const Texture &source, Vector2f position, Vector2f scale, double angle, Vector2f center, Rectangle2u shearRect)
    {
        Vector2u targetSize = this->getSize();
        Vector2u sourceSize = source.getSize();

        shearRect = {
            std::min(PMD::x(shearRect),      PMD::x(sourceSize)),
            std::min(PMD::y(shearRect),      PMD::y(sourceSize)),
            std::min(PMD::width(shearRect),  PMD::x(sourceSize) - PMD::x(shearRect)),
            std::min(PMD::height(shearRect), PMD::y(sourceSize) - PMD::y(shearRect))
        };

        bool negativeScaleX = PMD::x(scale) < 0.0;
        bool negativeScaleY = PMD::y(scale) < 0.0;

        scale = {
            std::abs(PMD::x(scale)),
            std::abs(PMD::y(scale))
        };

        position = {
            PMD::x(position) - (negativeScaleX ? PMD::width(shearRect) : 0.0),
            PMD::y(position) - (negativeScaleY ? PMD::height(shearRect) : 0.0)
        };

        double thetha = angle * std::numbers::pi / 180.0;

        double sinThetha = std::sin(thetha);
        double cosThetha = std::cos(thetha);

        Vector2f startPoint{ 0, 0 };
        Vector2f endPoint{ 0, 0 };

        for (const Vector2f &point : std::vector<Vector2f>{
            {0.0,                                   0.0},
            {PMD::width(shearRect) * PMD::x(scale), 0.0},
            {0.0,                                   PMD::height(shearRect) * PMD::y(scale)},
            {PMD::width(shearRect) * PMD::x(scale), PMD::height(shearRect) * PMD::y(scale)}
        }) {
            Vector2f rotatedPoint = {
                (PMD::x(point) - PMD::x(center)) * cosThetha - (PMD::y(point) - PMD::y(center)) * sinThetha,
                (PMD::x(point) - PMD::x(center)) * sinThetha + (PMD::y(point) - PMD::y(center)) * cosThetha
            };

            startPoint = {
                std::min(PMD::x(startPoint), PMD::x(rotatedPoint)),
                std::min(PMD::y(startPoint), PMD::y(rotatedPoint))
            };

            endPoint = {
                std::max(PMD::x(endPoint), PMD::x(rotatedPoint)),
                std::max(PMD::y(endPoint), PMD::y(rotatedPoint))
            };
        }

        startPoint = {
            PMD::x(startPoint) + PMD::x(position),
            PMD::y(startPoint) + PMD::y(position)
        };

        endPoint = {
            PMD::x(endPoint) + PMD::x(position),
            PMD::y(endPoint) + PMD::y(position)
        };

        this->access([=, this, &source](Color *targetContents) {
            source.access([=, this](const Color *sourceContents) {
                for (double y = PMD::y(startPoint); y < PMD::y(endPoint); y++)
                    for (double x = PMD::x(startPoint); x < PMD::x(endPoint); x++) {
                        Vector2f sourcePos{
                            negativeScaleX ?
                                (PMD::width(shearRect)  - 1 - (x - PMD::x(position)) / PMD::x(scale)) :
                                (x - PMD::x(position)) / PMD::x(scale),
                            negativeScaleY ?
                                (PMD::height(shearRect) - 1 - (y - PMD::y(position)) / PMD::y(scale)) :
                                (y - PMD::y(position)) / PMD::y(scale)
                        };

                        sourcePos = {
                            PMD::x(shearRect) + PMD::x(sourcePos) * cosThetha + PMD::y(sourcePos) * sinThetha + PMD::x(center) / PMD::x(scale),
                            PMD::y(shearRect) - PMD::x(sourcePos) * sinThetha + PMD::y(sourcePos) * cosThetha + PMD::y(center) / PMD::y(scale)
                        };
                        
                        Color sourceColor;
                        switch (this->_filtering) {
                            case Texture::Filtering::NEAREST: {
                                if (PMD::x(sourcePos) < PMD::x(shearRect) || PMD::x(sourcePos) >= PMD::x(shearRect) + PMD::width(shearRect) ||
                                    PMD::y(sourcePos) < PMD::y(shearRect) || PMD::y(sourcePos) >= PMD::y(shearRect) + PMD::height(shearRect))
                                    continue;

                                sourceColor =
                                    sourceContents[
                                        static_cast<::size_t>(std::floor(PMD::y(sourcePos))) * PMD::x(sourceSize)
                                      + static_cast<::size_t>(std::floor(PMD::x(sourcePos)))
                                    ];
                                break;
                            }

                            case Texture::Filtering::LINEAR: {
                                #define SOURCE_OR_TARGET(X, Y) \
                                   (X >= PMD::x(shearRect) && X < PMD::x(shearRect) + PMD::width(shearRect)  && X < PMD::x(sourceSize) && \
                                    Y >= PMD::y(shearRect) && Y < PMD::y(shearRect) + PMD::height(shearRect) && Y < PMD::y(sourceSize)) \
                                      ? sourceContents[static_cast<::size_t>((Y) * PMD::x(sourceSize) + (X))] \
                                      : Color{0, 0, 0, 0}

                                Color hColor = Color::lerp(
                                    SOURCE_OR_TARGET(std::floor(PMD::x(sourcePos)), std::floor(PMD::y(sourcePos))),
                                    SOURCE_OR_TARGET(std::ceil(PMD::x(sourcePos)), std::floor(PMD::y(sourcePos))),
                                    PMD::x(sourcePos) - std::floor(PMD::x(sourcePos))
                                );
                                
                                Color vColor = Color::lerp(
                                    SOURCE_OR_TARGET(std::floor(PMD::x(sourcePos)), std::floor(PMD::y(sourcePos))),
                                    SOURCE_OR_TARGET(std::floor(PMD::x(sourcePos)), std::ceil(PMD::y(sourcePos))),
                                    PMD::y(sourcePos) - std::floor(PMD::y(sourcePos))
                                );

                                #undef SOURCE_OR_TARGET

                                sourceColor = Color::lerp(hColor, vColor, 0.5);

                                break;
                            }
                        }

                        switch (this->_filtering) {
                            case Texture::Filtering::NEAREST: {
                                if (std::floor(x) < 0.0 || std::floor(x) >= PMD::x(targetSize) ||
                                    std::floor(y) < 0.0 || std::floor(y) >= PMD::y(targetSize))
                                    break;

                                Color *target = &targetContents[static_cast<::size_t>(
                                    std::floor(x) + (PMD::x(targetSize) * std::floor(y))
                                )];

                                *target = Color::lerp(*target, sourceColor, 1.0);
                                break;
                            }

                            case Texture::Filtering::LINEAR: {
                                double fracX = x - std::floor(x);
                                double fracY = y - std::floor(y);

                                for (const auto &[position, frac] : std::vector<std::pair<Vector2f, double>>{
                                    {Vector2f{std::floor(x), std::floor(y)}, (1.0 - fracX) * (1.0 - fracY)},
                                    {Vector2f{std::floor(x), std::ceil(y)},  (1.0 - fracX) * (      fracY)},
                                    {Vector2f{std::ceil(x), std::floor(y)},  (      fracX) * (1.0 - fracY)},
                                    {Vector2f{std::ceil(x), std::ceil(y)},   (      fracX) * (      fracY)}
                                }) {
                                    if (PMD::x(position) < 0.0 || PMD::x(position) >= PMD::x(targetSize) ||
                                        PMD::y(position) < 0.0 || PMD::y(position) >= PMD::y(targetSize))
                                        continue;

                                    Color *target = &targetContents[static_cast<::size_t>(
                                        PMD::x(position) + (PMD::x(targetSize) * PMD::y(position))
                                    )];

                                    // FIXME: Not correct, but it's a start...
                                    *target = Color::lerp(*target, sourceColor, frac * 2.0);
                                }

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
