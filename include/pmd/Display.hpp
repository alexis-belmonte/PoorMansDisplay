#pragma once

#include "pmd/DisplayController.hpp"
#include "pmd/DisplayInstanceManager.hpp"

#include "pmd/Event.hpp"
#include "pmd/EventQueue.hpp"
#include "pmd/InputManager.hpp"

#include "pmd/Texture.hpp"
#include "pmd/Vector.hpp"

#include <string>
#include <fstream>
#include <memory>

namespace PMD
{
    class Display
    {
        friend class DisplayInstanceManager;

    public:
        Display(const std::string &target = "/dev/tty");
        ~Display();

        Vector2u getScreenSize() const;
        Vector2u getFramebufferSize() const;

        void setIconTitle(const std::string &title) const;
        void setWindowTitle(const std::string &title) const;

    protected:
        void sendCommand(const std::string &feedback) const;
        std::string getFeedback() const;

        void requestFramebufferResize();
        void resizeFramebuffer();

    public:
        Texture &getFramebuffer();

        template<typename... Args>
        void poll(Args &&...args)
        {
            this->_eventQueue.poll(std::forward<Args>(args)...);
        }

        void update();
        void present();

    protected:
        int _fd;
        DisplayController _controller;
 
        Vector2u _lastFramebufferSize;
        bool _framebufferResizeRequested = false;
        std::unique_ptr<Texture> _framebuffer;
        std::unique_ptr<Texture> _framebufferUpdateMask;

        EventQueue _eventQueue;
        InputManager _inputManager;
    };
};
