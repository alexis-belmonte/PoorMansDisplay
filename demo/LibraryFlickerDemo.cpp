#include <pmd/Display.hpp>

#include <memory>

int main(void)
{
    std::unique_ptr<PMD::Display> display(std::make_unique<PMD::Display>());

    display->setWindowTitle("Hello, World!");

    bool running = true;

    while (running)
    {
        display->update();
        display->present();
        
        display->poll(
            PMD::EventQueue::Handler<PMD::DisplayCloseEvent>(
                [&running](const PMD::DisplayCloseEvent &)
                {
                    running = false;
                }
            ),
            PMD::EventQueue::Handler<PMD::KeyDownEvent>(
                [&running, &display](const PMD::KeyDownEvent &event)
                {
                    if (event.key == 'q')
                        running = false;
                    if (event.key == 'r') {
                        display.reset();
                        display = std::make_unique<PMD::Display>();
                        display->setWindowTitle("Hello, World! Reloaded!");
                    }
                }
            )
        );
    }

    return 0;
}
