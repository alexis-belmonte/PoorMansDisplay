#include <pmd/Display.hpp>
#include <pmd/Vector.hpp>
#include <pmd/Texture.hpp>
#include <pmd/TextureLoader.hpp>

#include <iostream>

#include <unistd.h>

static void setTextureColor(PMD::Texture &target, PMD::Texture &source, PMD::Color color)
{
    PMD::Vector2u size = source.getSize();

    target.access([&source, color, size](PMD::Color *targetContents) {
        source.access([targetContents, color, size](PMD::Color *sourceContents) {
            for (::size_t i = 0; i < std::get<0>(size) * std::get<1>(size); i++) {
                targetContents[i].c.a = sourceContents[i].c.a;
                targetContents[i].c.r = sourceContents[i].c.r * 1.0 * color.c.r;
                targetContents[i].c.g = sourceContents[i].c.g * 1.0 * color.c.g;
                targetContents[i].c.b = sourceContents[i].c.b * 1.0 * color.c.b;
            }
        });
    });
}

int main(void)
{
    PMD::Display  display;
    PMD::Texture  &framebuffer = display.getFramebuffer();
    PMD::Vector2u size = framebuffer.getSize();
    
    PMD::Texture  dvdLogoTexture(PMD::TextureLoader::fromLocalStorage("./DVDLogo.png"));
    PMD::Vector2u dvdLogoSize = dvdLogoTexture.getSize();

    PMD::Texture  dvdLogoColoredTexture(dvdLogoSize);

    setTextureColor(dvdLogoColoredTexture, dvdLogoTexture, ::rand());

    ::size_t x = ::rand() % (std::get<0>(size) - std::get<0>(dvdLogoSize));
    ::size_t y = ::rand() % (std::get<1>(size) - std::get<1>(dvdLogoSize));
    ::size_t xd = 1;
    ::size_t yd = 1; 

    for (int i = 0; i < 1000; i++) {
        display.update();

        bool hasMovedDirection = false;

        if (x + xd > std::get<0>(size) - std::get<0>(dvdLogoSize) || x + xd <= 0) {
            xd = -xd;
            hasMovedDirection = true;
        }
        if (y + yd > std::get<1>(size) - std::get<1>(dvdLogoSize) || y + yd <= 0) {
            yd = -yd;
            hasMovedDirection = true;
        }

        if (hasMovedDirection)
            setTextureColor(dvdLogoColoredTexture, dvdLogoTexture, ::rand());

        x += xd;
        y += yd;
    
        framebuffer.clear(0, 0, 0, 255);
        framebuffer.blit({x, y}, dvdLogoColoredTexture);
        display.present();

        ::usleep(15000);
    }
}
