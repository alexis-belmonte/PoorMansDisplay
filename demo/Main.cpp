#include <pmd/Display.hpp>
#include <pmd/Vector.hpp>

#include <iostream>

#include <unistd.h>

int main(void)
{
    PMD::Display display;
    PMD::Texture &framebuffer = display.getFramebuffer();
    PMD::Vector2u size = framebuffer.getSize();
    
    PMD::Texture myTexture({35, 17});
    PMD::Vector2u textSize = myTexture.getSize();
    myTexture.clear(::rand());

    size_t x = ::rand() % (std::get<0>(size) - std::get<0>(textSize));
    size_t y = ::rand() % (std::get<1>(size) - std::get<1>(textSize));
    size_t xd = 1;
    size_t yd = 1; 

    for (int i = 0; i < 1000; i++) {
        display.update();

        /*
        framebuffer.access([&framebuffer, i](::uint32_t *contents) {
            PMD::Vector2u size = framebuffer.getSize();

            for (::size_t y = 0; y < std::get<1>(size); y++) {
                for (::size_t x = 0; x < std::get<0>(size); x++)
                    contents[y * std::get<0>(size) + x] =
                        (0xff       << 24) |
                        (((i + x) % 0xff) << 16) |
                        (((i + y) % 0xff) <<  8) |
                        (((i + (x | y)) % 0xff));
            }
        });
        */

        bool hasMovedDirection = false;

        if (x + xd > std::get<0>(size) - std::get<0>(textSize) || x + xd < 0) {
            xd = -xd;
            hasMovedDirection = true;
        }
        if (y + yd > std::get<1>(size) - std::get<1>(textSize) || y + yd < 0) {
            yd = -yd;
            hasMovedDirection = true;
        }

        if (hasMovedDirection)
            myTexture.clear(::rand());

        x += xd;
        y += yd;
    
        framebuffer.clear(0xff000000);
        framebuffer.blit({x, y}, myTexture);
        display.present();

        ::usleep(15000);
    }
}
