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
    PMD::Vector2u redSquarePos = {0, 0};

    redSquare.clear(0, 0, 0, 127);

    display.setWindowTitle("Input Demo");

    while (running) {
        display.update();

        display.poll(
            PMD::EventQueue::Handler<PMD::KeyDownEvent>(
                [&running, &redSquare](const PMD::KeyDownEvent &event) {
                    if (event.key == 'q') {
                        running = false;
                        return;
                    }
                    if (event.key != 'a')
                        return;
                    redSquare.clear(255, 0, 0, 127);
                }
            ),
            PMD::EventQueue::Handler<PMD::KeyUpEvent>(
                [&redSquare](const PMD::KeyUpEvent &event) {
                    if (event.key != 'a')
                        return;
                    redSquare.clear(0, 0, 0, 127);
                }
            )
        );

        framebuffer.clear(127, 127, 127);
        framebuffer.blit(redSquare, redSquarePos);

        display.present();

        redSquarePos = {PMD::x(redSquarePos) + 1, PMD::y(redSquarePos) + 1};
        if (PMD::y(redSquarePos) > PMD::y(framebuffer.getSize()) - PMD::y(redSquare.getSize()))
            redSquarePos = {0, 0};

        std::this_thread::sleep_for(16ms);
    }

    return 0;
}
