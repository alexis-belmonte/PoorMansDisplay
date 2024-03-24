#include <pmd/Display.hpp>

#include <chrono>
#include <thread>

int main(void)
{
    using namespace std::chrono_literals;

    PMD::Display display;
    bool running = true;

    PMD::Texture &framebuffer = display.getFramebuffer();
    PMD::Texture redSquare(10, 10);
    PMD::Vector2f redSquarePos = {0.0, 0.0};

    redSquare.clear(0, 0, 0, 127);

    display.setWindowTitle("Input Demo");
    framebuffer.setFiltering(PMD::Texture::Filtering::LINEAR);
    
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    while (running) {
        display.update();

        display.poll(
            PMD::EventQueue::Handler<PMD::KeyDownEvent>(
                [&](const PMD::KeyDownEvent &event) {
                    if (event.key == 'q') {
                        running = false;
                        return;
                    }
                    if (event.key == 'a')
                        redSquare.clear(255, 0, 0, 127);

                    if (event.key == 'i')
                        up = true;
                    if (event.key == 'k')
                        down = true;
                    if (event.key == 'j')
                        left = true;
                    if (event.key == 'l')
                        right = true;
                }
            ),
            PMD::EventQueue::Handler<PMD::KeyUpEvent>(
                [&](const PMD::KeyUpEvent &event) {
                    if (event.key == 'a')
                        redSquare.clear(0, 0, 0, 127);

                    if (event.key == 'i')
                        up = false;
                    if (event.key == 'k')
                        down = false;
                    if (event.key == 'j')
                        left = false;
                    if (event.key == 'l')
                        right = false;
                }
            )
        );

        redSquarePos = {
            (PMD::x(redSquarePos) + (right - left) * 0.25),
            (PMD::y(redSquarePos) + (down - up) * 0.25)
        };

        framebuffer.clear(127, 127, 127);
        framebuffer.blit(redSquare, redSquarePos);

        display.present();

        std::this_thread::sleep_for(16ms);
    }

    return 0;
}
