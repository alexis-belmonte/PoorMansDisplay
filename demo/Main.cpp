#include <pmd/Display.hpp>
#include <pmd/Vector.hpp>

#include <iostream>

#include <unistd.h>

int main(void)
{
    PMD::Display display;
    PMD::Texture &framebuffer = display.getFramebuffer();

    PMD::Texture myTexture({10, 10});
    myTexture.clear(0xff0e24e1);

    /*
    for (int i = 0; i < 32; i++) {
        myTexture.clear(
            (0xff             << 24) |
            (((16 + i * 4) % 256) << 16) |
            (((32 + i * 4) % 256) <<  8) |
            (((48 + i * 4) % 256) <<  0)
        );
        framebuffer.blit({i, i}, myTexture);
    }
    */

    size_t x = 0;
    size_t y = 0;
    size_t xd = 1;
    size_t yd = 1;

    for (int i = 0; i < 200; i++) {
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

        PMD::Vector2u size = framebuffer.getSize();
        PMD::Vector2u textSize = myTexture.getSize();

        if (x + xd > std::get<0>(size) - std::get<0>(textSize) || x + xd < 0)
            xd = -xd;
        if (y + yd > std::get<1>(size) - std::get<0>(textSize) || y + yd < 0)
            yd = -yd;

        x += xd;
        y += yd;

        framebuffer.clear(0xff005000);
        framebuffer.blit({x, y}, myTexture);

        display.present();

        ::usleep(8500);
    }
}
