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
        _controller(this->_fd),
        _inputManager(this->_eventQueue)
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

    Vector2u Display::getScreenSize() const
    {
        struct ::winsize size;

        if (::ioctl(this->_fd, TIOCGWINSZ, &size) != 0)
            throw std::runtime_error("Failed to get terminal size");

        return Vector2u(size.ws_col + 1, size.ws_row);
    }

    Vector2u Display::getFramebufferSize() const
    {
        Vector2u termSize = this->getScreenSize();

        return Vector2u{PMD::x(termSize), PMD::y(termSize) * 2};
    }

    void Display::setIconTitle(const std::string &title) const
    {
        this->sendCommand(std::format(EscapeSequence::SET_ICON_TITLE, title));
    }

    void Display::setWindowTitle(const std::string &title) const
    {
        this->sendCommand(std::format(EscapeSequence::SET_WINDOW_TITLE, title));
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
            return {};

        char buffer[available];

        if (::read(this->_fd, buffer, available) < 0)
            throw std::runtime_error("Failed to read from the terminal");

        return std::string(buffer, available);
    }

    void Display::requestFramebufferResize()
    {
        this->_framebufferResizeRequested = true;
    }

    void Display::resizeFramebuffer()
    {
        Vector2u newSize = this->getFramebufferSize();

        if (newSize == this->_lastFramebufferSize)
            return;

        if (!this->_framebuffer)
            this->_framebuffer = std::make_unique<Texture>(newSize);
        if (!this->_framebufferUpdateMask)
            this->_framebufferUpdateMask = std::make_unique<Texture>(newSize);

        this->_framebuffer->resize(newSize);

        this->_framebufferUpdateMask->resize(newSize);
        this->_framebufferUpdateMask->clear();

        this->_lastFramebufferSize = newSize;

        this->sendCommand(EscapeSequence::CLEAR_SCREEN);
        this->sendCommand(EscapeSequence::CLEAR_SCROLLBACK);
        this->sendCommand(EscapeSequence::SET_CURSOR_HOME);

        this->_eventQueue.push<DisplayResizeEvent>(newSize);

        this->_framebufferRedrawRequested = true;
    }
    
    Texture &Display::getFramebuffer()
    {
        return *this->_framebuffer;
    }

    void Display::update()
    {
        if (this->_framebufferResizeRequested) {
            this->resizeFramebuffer();
            this->_framebufferResizeRequested = false;
        }

        this->_inputManager.feed(this->getFeedback());
    }

    void Display::present()
    {
        std::stringstream commandStream;

        this->_framebufferUpdateMask->access([this, &commandStream](Color *mask) {
            this->_framebuffer->access([this, mask, &commandStream](Color *contents) {
                Vector2u size = this->_framebuffer->getSize();
                Vector2u currPos{~0, ~0};

                for (::size_t y = 0; y < PMD::y(size); y += 2) {
                    for (::size_t x = 0; x < PMD::x(size); x++) {
                        size_t i1 =  y      * PMD::x(size) + x;
                        size_t i2 = (y + 1) * PMD::x(size) + x;

                        if (!this->_framebufferRedrawRequested &&
                            !(mask[i1].v ^ contents[i1].v) && !(mask[i2].v ^ contents[i2].v))
                            continue;

                        if (PMD::x(currPos) != x || PMD::y(currPos) != y / 2) {
                            commandStream << "\e[" << (1 + y / 2) << ";" << (1 + x) << "H";
                            currPos = {x, y / 2};
                        }

                        commandStream << "\e[38;2;" <<
                                             static_cast<int>(contents[i1].c.r) << ";" <<
                                             static_cast<int>(contents[i1].c.g) << ";" <<
                                             static_cast<int>(contents[i1].c.b) << "m"
                                      << "\e[48;2;" <<
                                             static_cast<int>(contents[i2].c.r) << ";" <<
                                             static_cast<int>(contents[i2].c.g) << ";" <<
                                             static_cast<int>(contents[i2].c.b) << "m"
                                      << "▀";
                        
                        currPos = {PMD::x(currPos) + 1, PMD::y(currPos)};
                    }
                }
            });
        });

        this->_framebufferUpdateMask->copy(*this->_framebuffer);

        const std::string &command = commandStream.str();
        ::write(this->_fd, command.c_str(), command.size());

        this->_framebufferRedrawRequested = false;
    }
};
