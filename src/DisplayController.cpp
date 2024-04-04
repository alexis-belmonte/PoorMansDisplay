#include "pmd/DisplayController.hpp"
#include "pmd/EscapeSequence.hpp"

#include <stdexcept>

#include <unistd.h>

#include <sys/ioctl.h>

namespace PMD
{
    DisplayController::DisplayController(int fd) :
        _fd(fd)
    {
        if (::ioctl(fd, TIOCEXCL, 0) < 0)
            throw std::runtime_error("Failed to set exclusive mode on the terminal");
    }

    DisplayController::~DisplayController()
    {
        ::ioctl(this->_fd, TIOCNXCL, 0);
        ::close(this->_fd);
    }

    void DisplayController::pushContext()
    {
        ::termios context;
        if (::tcgetattr(this->_fd, &context) != 0)
            throw std::runtime_error("Failed to get terminal context");

        this->_ttyContext.push_back(context);
    }

    void DisplayController::popContext()
    {
        if (this->_ttyContext.empty())
            throw std::runtime_error("No terminal context to pop");

        ::termios context = this->_ttyContext.back();
        this->_ttyContext.pop_back();

        if (::tcsetattr(this->_fd, TCSANOW, &context) != 0)
            throw std::runtime_error("Failed to restore terminal context from stack");
    }
    
    void DisplayController::updateContext(std::function<void(::termios &)> &&updaterCallback)
    {
        if (this->_ttyContext.empty())
            throw std::runtime_error("No terminal context in the stack, cannot continue");

        ::termios &context = this->_ttyContext.back();
        updaterCallback(context);

        if (::tcsetattr(this->_fd, TCSANOW, &context) != 0)
            throw std::runtime_error("Failed to update terminal context");
    }

    void DisplayController::updateFlags(int flags, bool enable)
    {
        this->updateContext(
            [flags, enable](::termios &context)
            {
                if (enable)
                    context.c_lflag |= flags;
                else
                    context.c_lflag ^= flags;
            }
        );
    }

    bool DisplayController::areFlagsSet(int flags) const
    {
        if (this->_ttyContext.empty())
            throw std::runtime_error("No terminal context in the stack, cannot continue");

        return this->_ttyContext.back().c_lflag & flags;
    }
    
    void DisplayController::setInputEcho(bool enable)
    {
        this->updateFlags(ECHO, enable);
    }

    bool DisplayController::getInputEcho() const
    {
        return this->areFlagsSet(ECHO);
    }

    void DisplayController::setCanonical(bool enable)
    {
        this->updateFlags(ICANON, enable);
    }

    bool DisplayController::getCanonical() const
    {
        return this->areFlagsSet(ICANON);
    }
};
