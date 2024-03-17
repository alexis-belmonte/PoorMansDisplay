#include "pmd/DisplayInstanceManager.hpp"

#include "pmd/Display.hpp"

#include "pmd/Event.hpp"
#include "pmd/Vector.hpp"

#include <signal.h>

namespace PMD
{
    void DisplayInstanceManager::setup()
    {
        ::signal(SIGWINCH, [](int)
        {
            DisplayInstanceManager::foreach([](Display *display)
            {
                display->requestFramebufferResize();
            });
        });

        ::signal(SIGINT, [](int)
        {
            DisplayInstanceManager::foreach([](Display *display)
            {
                display->getEventQueue().push<DisplayCloseEvent>();
            });
        });
    }

    void DisplayInstanceManager::teardown()
    {
        ::signal(SIGWINCH, SIG_DFL);
        ::signal(SIGINT, SIG_DFL);
    }

    void DisplayInstanceManager::add(Display *display)
    {
        if (DisplayInstanceManager::_displays.size() == 0)
            DisplayInstanceManager::setup();

        DisplayInstanceManager::_displays.push_back(display);
    }

    void DisplayInstanceManager::remove(Display *display)
    {
        std::remove(DisplayInstanceManager::_displays.begin(), DisplayInstanceManager::_displays.end(), display);

        if (DisplayInstanceManager::_displays.size() == 0)
            DisplayInstanceManager::teardown();
    }

    void DisplayInstanceManager::foreach(std::function<void(Display *display)> &&callback)
    {
        for (Display *display : DisplayInstanceManager::_displays)
            callback(display);
    }

    std::vector<Display *> DisplayInstanceManager::_displays;
};
