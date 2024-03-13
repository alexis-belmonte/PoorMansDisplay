#include <pmd/Display.hpp>
#include <pmd/Vector.hpp>

#include <iostream>

#include <unistd.h>

int main(void)
{
    PMD::Display display;
    PMD::Texture &framebuffer = display.getFramebuffer();

    framebuffer.clear(0xff005000);

    PMD::Texture myTexture({10, 10});

    for (int i = 0; i < 32; i++) {
        myTexture.clear(
            (0xff             << 24) |
            (((16 + i * 4) % 256) << 16) |
            (((32 + i * 4) % 256) <<  8) |
            (((48 + i * 4) % 256) <<  0)
        );
        framebuffer.blit({i, i}, myTexture);
    }

    /*
    framebuffer.access([&framebuffer](::uint32_t *contents) {
        PMD::Vector2u size = framebuffer.getSize();

        for (::size_t y = 0; y < std::get<1>(size); y++) {
            for (::size_t x = 0; x < std::get<0>(size); x++)
                contents[y * std::get<0>(size) + x] =
                    (0xff       << 24) |
                    ((x % 0xff) << 16) |
                    ((y % 0xff) <<  8) |
                    ((x | y) % 0xff);
        }
    });
    */

    for (int i = 0; i < 10; i++) {
        display.update();
        display.present();
        ::sleep(1);
    }
}
