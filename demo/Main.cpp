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
            for (::size_t i = 0; i < PMD::x(size) * PMD::y(size); i++) {
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
    PMD::Vector2u displaySize = display.getSize();
    
    PMD::Texture  dvdLogoTexture(PMD::TextureLoader::fromLocalStorage("./DVDLogo.png"));
    PMD::Vector2u dvdLogoSize = dvdLogoTexture.getSize();

    PMD::Texture  dvdLogoColoredTexture(dvdLogoSize);

    setTextureColor(dvdLogoColoredTexture, dvdLogoTexture, ::rand());

    ::size_t x;
    ::size_t y;
    ::size_t xd = 1;
    ::size_t yd = 1; 

    bool running = true;

    while (running) {
        display.update();

        display.getEventQueue().poll(
            PMD::EventQueue::Handler<PMD::DisplayResizeEvent>(
                [&x, &y, &displaySize, dvdLogoSize](const PMD::DisplayResizeEvent &event) {
                    displaySize = event.size;
                    x = ::rand() % (PMD::x(displaySize) - PMD::x(dvdLogoSize));
                    y = ::rand() % (PMD::y(displaySize) - PMD::y(dvdLogoSize));
                }
            ),
            PMD::EventQueue::Handler<PMD::DisplayCloseEvent>(
                [&running](const PMD::DisplayCloseEvent &) {
                    running = false;
                }
            )
        );
        
        bool hasMovedDirection = false;

        if (x + xd > PMD::x(displaySize) - PMD::x(dvdLogoSize) || x + xd <= 0) {
            xd = -xd;
            hasMovedDirection = true;
        }
        if (y + yd > PMD::y(displaySize) - PMD::y(dvdLogoSize) || y + yd <= 0) {
            yd = -yd;
            hasMovedDirection = true;
        }

        if (hasMovedDirection)
            setTextureColor(dvdLogoColoredTexture, dvdLogoTexture, ::rand());

        x += xd;
        y += yd;

        display.access([x, y, &dvdLogoColoredTexture](PMD::Texture &framebuffer) {
            framebuffer.clear(0, 0, 0, 255);
            framebuffer.blit({x, y}, dvdLogoColoredTexture);
        });
        display.present();

        ::usleep(15000);
    }
}
