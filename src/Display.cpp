#include "pmd/Display.hpp"
#include "pmd/EscapeSequence.hpp"

#include <format>
#include <stdexcept>

#include <cstring>

#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>

namespace PMD
{
    Display::Display(const std::string &target) :
        _fd(
            [&target]() -> int
            {
                int fd;

                if ((fd = ::open(target.c_str(), O_RDWR)) < 0)
                    throw std::runtime_error(
                        std::format("Failed to open display target '{}': {}", target, ::strerror(errno))
                    );
                return fd;
            }()
        ),
        _controller(this->_fd)
    {
        this->_controller.setInputEcho(false);
        this->_controller.setCanonical(false);
    }

    Display::~Display()
    {
    }

    Vector2u Display::getSize() const
    {
        struct ::winsize size;

        if (::ioctl(this->_fd, TIOCGWINSZ, &size) != 0)
            throw std::runtime_error("Failed to get terminal size");

        return Vector2u(size.ws_col, size.ws_row);
    }

    std::string Display::getFeedback() const
    {
        size_t available;

        if (::ioctl(this->_fd, FIONREAD, &available) != 0)
            throw std::runtime_error("Failed to get available byte count from the terminal");
        
        char buffer[available];

        if (::read(this->_fd, buffer, available) < 0)
            throw std::runtime_error("Failed to read from the terminal");

        return std::string(buffer, available);
    }

    Vector2u Display::getCursorPosition() const
    {
        ::write(this->_fd, EscapeSequence::GET_CURSOR_POSITION, sizeof(EscapeSequence::GET_CURSOR_POSITION));

        std::string feedback(this->getFeedback());
        if (!(feedback.starts_with("\e[") && feedback.ends_with('R')))
            throw std::runtime_error("Failed to get cursor position");

        int feedbackSep = feedback.find(';');
        return Vector2u{
            std::atoi(feedback.substr(2, feedbackSep).c_str()),
            std::atoi(feedback.substr(feedbackSep + 1, feedback.size() - 2).c_str())
        };
    }

    void Display::setCursorPosition(const Vector2u &position)
    {
        std::string command(
            std::format(EscapeSequence::SET_CURSOR_POSITION,
                std::get<0>(position), std::get<1>(position))
        );
    }
};
