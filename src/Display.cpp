#include "pmd/Display.hpp"

#include "pmd/DisplayInstanceManager.hpp"

#include "pmd/EscapeSequence.hpp"

#include <format>
#include <stdexcept>
#include <sstream>

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

        this->sendCommand(EscapeSequence::FLIP_SCREEN_ALTERNATE);
        this->sendCommand(EscapeSequence::HIDE_CURSOR_CARET);
        this->sendCommand(EscapeSequence::DISABLE_AUTOWRAP);

        this->resizeFramebuffer();
        
        DisplayInstanceManager::add(this);
    }

    Display::~Display()
    {
        this->sendCommand(EscapeSequence::RESET_TERMINAL);
        this->sendCommand(EscapeSequence::FLIP_SCREEN_NORMAL);
        this->sendCommand(EscapeSequence::RESET_TERMINAL);

        DisplayInstanceManager::remove(this);
    }

    Vector2u Display::getSize() const
    {
        struct ::winsize size;

        if (::ioctl(this->_fd, TIOCGWINSZ, &size) != 0)
            throw std::runtime_error("Failed to get terminal size");

        return Vector2u(size.ws_col + 1, size.ws_row + 1);
    }
    
    void Display::sendCommand(const std::string &command) const
    {
        if (::write(this->_fd, command.c_str(), command.size()) < 0)
            throw std::runtime_error("Failed to send command to the terminal");
    }

    std::string Display::getFeedback() const
    {
        int available;

        if (::ioctl(this->_fd, FIONREAD, &available) != 0)
            throw std::runtime_error("Failed to get available byte count from the terminal");

        if (available == 0)
            return "";

        char buffer[available];

        if (::read(this->_fd, buffer, available) < 0)
            throw std::runtime_error("Failed to read from the terminal");

        return std::string(buffer, available);
    }

    void Display::requestFramebufferResize()
    {
        this->_framebufferResizeRequested = true;
    }

    void Display::resizeFramebuffer(Vector2u newSize)
    {
        if (newSize == this->_lastFramebufferSize)
            return;

        if (!this->_framebuffer)
            this->_framebuffer = std::make_unique<Texture>(newSize);
        if (!this->_framebufferUpdateMask)
            this->_framebufferUpdateMask = std::make_unique<Texture>(newSize);

        this->_framebuffer->resize(newSize);
        this->_framebufferUpdateMask->resize(newSize);

        this->_framebufferUpdateMask->copy(Vector2u{0, 0}, *this->_framebuffer);
        this->_framebufferUpdateMask->negate();

        this->_lastFramebufferSize = newSize;

        this->sendCommand(EscapeSequence::CLEAR_SCREEN);
        this->sendCommand(EscapeSequence::CLEAR_SCROLLBACK);
        this->sendCommand(EscapeSequence::SET_CURSOR_HOME);

        this->_eventQueue.push<DisplayResizeEvent>(newSize);
    }
    
    void Display::resizeFramebuffer()
    {
        Vector2u termSize = this->getSize();

        this->resizeFramebuffer(
            Vector2u{std::get<0>(termSize), std::get<1>(termSize) * 2}
        );
    }

    Texture &Display::getFramebuffer()
    {
        return *this->_framebuffer;
    }

    EventQueue &Display::getEventQueue()
    {
        return this->_eventQueue;
    }

    void Display::update()
    {
        if (this->_framebufferResizeRequested) {
            Vector2u termSize = this->getSize();
            if (termSize == this->_lastFramebufferSize)
                return;

            this->resizeFramebuffer();
            this->_framebufferResizeRequested = false;
        }

        std::string feedback = this->getFeedback();

        if (feedback.empty())
            return;

        // TODO: Manage feedback
    }

    void Display::present()
    {
        std::stringstream commandStream;

        this->_framebufferUpdateMask->access([this, &commandStream](Color *mask) {
            this->_framebuffer->access([this, mask, &commandStream](Color *contents) {
                Vector2u size = this->_framebuffer->getSize();
                Vector2u currPos{~0, ~0};

                for (::size_t y = 0; y < std::get<1>(size); y += 2) {
                    for (::size_t x = 0; x < std::get<0>(size); x++) {
                        size_t i1 =  y      * std::get<0>(size) + x;
                        size_t i2 = (y + 1) * std::get<0>(size) + x;

                        if (!(mask[i1].v ^ contents[i1].v) && !(mask[i2].v ^ contents[i2].v))
                            continue;

                        if (std::get<0>(currPos) != x || std::get<1>(currPos) != y) {
                            commandStream << std::format(EscapeSequence::SET_CURSOR_POSITION, y / 2, x);
                            currPos = {x, y / 2};
                        }

                        commandStream << std::format(EscapeSequence::SET_RGB_FGCOLOR,
                                             contents[i1].c.r,
                                             contents[i1].c.g,
                                             contents[i1].c.b)
                                      << std::format(EscapeSequence::SET_RGB_BGCOLOR,
                                             contents[i2].c.r,
                                             contents[i2].c.g,
                                             contents[i2].c.b)
                                      << "▀";
                        
                        currPos = {std::get<0>(currPos) + 1, std::get<1>(currPos)};
                    }
                    currPos = {0, std::get<1>(currPos) + 1};
                }
            });
        });

        this->_framebufferUpdateMask->copy(Vector2u{0, 0}, *this->_framebuffer);

        const std::string &command = commandStream.str();
        ::write(this->_fd, command.c_str(), command.size());
    }
};
