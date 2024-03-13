#include "pmd/Display.hpp"
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
        this->sendCommand(EscapeSequence::CLEAR_SCREEN);
        this->sendCommand(EscapeSequence::SET_CURSOR_HOME);
        
        this->resizeFramebuffer();
    }

    Display::~Display()
    {
        this->sendCommand(EscapeSequence::RESET_TERMINAL);
        this->sendCommand(EscapeSequence::FLIP_SCREEN_NORMAL);
        this->sendCommand(EscapeSequence::RESET_TERMINAL);
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

    void Display::resizeFramebuffer(Vector2u newSize)
    {
        this->_framebuffer           = std::make_unique<Texture>(newSize);
        this->_framebufferUpdateMask = std::make_unique<Texture>(newSize);

        this->_framebufferUpdateMask->negate();
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

    void Display::update()
    {
        std::string feedback = this->getFeedback();

        if (feedback.empty())
            return;

        // TODO: Manage feedback
        // TODO: Implement framebuffer update based on signal
    }

    void Display::present()
    {
        std::stringstream commandStream;

        this->_framebufferUpdateMask->access([this, &commandStream](::uint32_t *mask) {
            this->_framebuffer->access([this, mask, &commandStream](::uint32_t *contents) {
                Vector2u size = this->_framebuffer->getSize();
                Vector2u currPos{~0, ~0};

                for (::size_t y = 0; y < std::get<1>(size); y += 2) {
                    for (::size_t x = 0; x < std::get<0>(size); x++) {
                        size_t i1 =  y      * std::get<0>(size) + x;
                        size_t i2 = (y + 1) * std::get<0>(size) + x;

                        if (!(mask[i1] ^ contents[i1]) && !(mask[i2] ^ contents[i2]))
                            continue;

                        if (std::get<0>(currPos) != x || std::get<1>(currPos) != y) {
                            commandStream << std::format(EscapeSequence::SET_CURSOR_POSITION, y / 2, x);
                            currPos = {x, y / 2};
                        }

                        commandStream << std::format(EscapeSequence::SET_RGB_FGCOLOR,
                                             ((contents[i1] >> 16) & 0xff),
                                             ((contents[i1] >>  8) & 0xff),
                                             ((contents[i1]      ) & 0xff))
                                      << std::format(EscapeSequence::SET_RGB_BGCOLOR,
                                             ((contents[i2] >> 16) & 0xff),
                                             ((contents[i2] >>  8) & 0xff),
                                             ((contents[i2]      ) & 0xff))
                                      << "▀";
                        
                        currPos = {std::get<0>(currPos) + 1, std::get<1>(currPos)};
                    }
                    currPos = {0, std::get<1>(currPos) + 1};
                }
            });
        });

        this->_framebufferUpdateMask->blit(Vector2u{0, 0}, *this->_framebuffer);

        const std::string &command = commandStream.str();
        ::write(this->_fd, command.c_str(), command.size());
    }
};
