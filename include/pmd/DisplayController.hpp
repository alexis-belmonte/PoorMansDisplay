#pragma once

#include <vector>
#include <functional>

#include <termios.h>

namespace PMD
{
    class DisplayController
    {
    public:
        DisplayController(int fd);
        ~DisplayController();

        void pushContext();
        void popContext();
        void updateContext(std::function<void(::termios &)> &&updaterCallback);

    protected:
        void updateFlags(int flags, bool enable);
        bool areFlagsSet(int flags) const;

    public:
        void setInputEcho(bool enable);
        bool getInputEcho() const;

        void setCanonical(bool enable);
        bool getCanonical() const;

    protected:
        int _fd;
        std::vector<::termios> _ttyContext;
    };
};
