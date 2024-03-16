#pragma once

#include <functional>
#include <vector>

namespace PMD
{
    class Display;

    class DisplayInstanceManager
    {
    protected:
        static void setup();
        static void teardown();

    public:
        static void add(Display *display);
        static void remove(Display *display);

        static void foreach(std::function<void(Display *display)> &&callback);

    protected:
        static std::vector<Display *> _displays;
    };
};
