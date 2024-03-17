#pragma once

#include "pmd/DisplayController.hpp"
#include "pmd/DisplayInstanceManager.hpp"

#include "pmd/Event.hpp"
#include "pmd/EventQueue.hpp"

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

        Vector2u getSize() const;

    protected:
        void sendCommand(const std::string &feedback) const;
        std::string getFeedback() const;

        void requestFramebufferResize();
        void resizeFramebuffer(Vector2u newSize);
        void resizeFramebuffer();

    public:
        Texture &getFramebuffer();
        EventQueue &getEventQueue();

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
    };
};
