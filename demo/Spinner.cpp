#include <pmd/Display.hpp>
#include <pmd/TextureLoader.hpp>

#include <thread>

int main(void)
{
    using namespace std::chrono_literals;

    PMD::Display display;
    PMD::Texture &framebuffer = display.getFramebuffer();

    PMD::Texture spinnerTexture(PMD::TextureLoader::fromLocalStorage("Spinner.png"));
    const std::array<PMD::Rectangle2u, 8> spinnerTextureRects = {
        PMD::Rectangle2u(0, 0, 32, 32),
        PMD::Rectangle2u(32, 0, 32, 32),
        PMD::Rectangle2u(0, 32, 32, 32),
        PMD::Rectangle2u(32, 32, 32, 32),
        PMD::Rectangle2u(0, 64, 32, 32),
        PMD::Rectangle2u(32, 64, 32, 32),
        PMD::Rectangle2u(0, 96, 32, 32),
        PMD::Rectangle2u(32, 96, 32, 32),
    };
    ::size_t spinnerTextureIndex = 0;

    display.setWindowTitle("Spinner/Shear Rect Demo");

    framebuffer.setFiltering(PMD::Texture::Filtering::LINEAR);

    bool running = true;
    double i = 0.0;

    while (running) {
        display.update();

        display.poll(
            PMD::EventQueue::Handler<PMD::DisplayCloseEvent>(
                [&](const PMD::DisplayCloseEvent &) {
                    running = false;
                }
            )
        );

        framebuffer.clear();
        framebuffer.blit(spinnerTexture, {75, 75}, {1.2, 1.2}, i, {16, 16}, spinnerTextureRects.at(spinnerTextureIndex));

        spinnerTextureIndex = ++spinnerTextureIndex % spinnerTextureRects.size();
        
        display.present();

        std::this_thread::sleep_for(60ms);
        i += 1;
    }

    return 0;
}
