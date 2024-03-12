#pragma once

#include "pmd/DisplayController.hpp"
#include "pmd/Vector.hpp"

#include <string>
#include <fstream>

namespace PMD
{
    class Display
    {
    public:
        Display(const std::string &target = "/dev/tty");
        ~Display();

        Vector2u getSize() const;

    protected:
        std::string getFeedback() const;

        Vector2u getCursorPosition() const;
        void setCursorPosition(const Vector2u &position);

    protected:
        int _fd;

        DisplayController _controller;
    };
};
