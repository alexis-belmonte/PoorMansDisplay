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
                targetContents[i].c.r = sourceContents[i].c.r * 1.0 / 255.0 * color.c.r;
                targetContents[i].c.g = sourceContents[i].c.g * 1.0 / 255.0 * color.c.g;
                targetContents[i].c.b = sourceContents[i].c.b * 1.0 / 255.0 * color.c.b;
                targetContents[i].c.a = sourceContents[i].c.a;
            }
        });
    });
}

int main(void)
{
    PMD::Display  display;

    display.setWindowTitle("DVD Logo Demo");

    PMD::Texture  &framebuffer = display.getFramebuffer();
    PMD::Vector2u displaySize = display.getFramebufferSize();

    framebuffer.setFiltering(PMD::Texture::Filtering::LINEAR);
    
    PMD::Texture  dvdLogoTexture(PMD::TextureLoader::fromLocalStorage("./DVDLogo.png"));
    PMD::Vector2u dvdLogoSize = dvdLogoTexture.getSize();

    PMD::Texture  dvdLogoColoredTexture(dvdLogoSize);

    setTextureColor(dvdLogoColoredTexture, dvdLogoTexture, ::rand());

    double x;
    double y;
    double xd = 0.4;
    double yd = 0.4;

    bool running = true;

    while (running) {
        display.update();

        display.poll(
            PMD::EventQueue::Handler<PMD::DisplayResizeEvent>(
                [&x, &y, &displaySize, dvdLogoSize](const PMD::DisplayResizeEvent &event) {
                    displaySize = event.size;

                    if (PMD::x(dvdLogoSize) >= PMD::x(displaySize) ||
                        PMD::y(dvdLogoSize) >= PMD::y(displaySize)) {
                        x = 0.0;
                        y = 0.0;
                    } else {
                        x = ::rand() % (PMD::x(displaySize) - PMD::x(dvdLogoSize));
                        y = ::rand() % (PMD::y(displaySize) - PMD::y(dvdLogoSize));
                    }
                }
            ),
            PMD::EventQueue::Handler<PMD::DisplayCloseEvent>(
                [&running](const PMD::DisplayCloseEvent &) {
                    running = false;
                }
            ),
            PMD::EventQueue::Handler<PMD::KeyDownEvent>(
                [&running](const PMD::KeyDownEvent &event) {
                    if (event.key == 'q')
                        running = false;
                }
            )
        );
        
        if (PMD::x(displaySize) > PMD::x(dvdLogoSize) &&
            PMD::y(displaySize) > PMD::y(dvdLogoSize)) {
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
        }

        framebuffer.clear();
        framebuffer.blit(dvdLogoColoredTexture, {x, y}, {1.0, 1.0});
        display.present();

        ::usleep(15000);
    }

    return 0;
}
