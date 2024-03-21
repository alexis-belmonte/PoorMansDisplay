#pragma once

#include "pmd/Vector.hpp"
#include "pmd/Key.hpp"

#include <string>

#include <cstddef>

namespace PMD
{
    struct Event
    {
        virtual ~Event() = default;
    };

    struct DisplayCloseEvent : public Event
    {};

    struct DisplayResizeEvent : public Event
    {
        DisplayResizeEvent(Vector2u size) :
            size(size)
        {}

        Vector2u size;
    };

    struct KeyEvent : public Event, public Key
    {
        KeyEvent(char key, bool shift, bool ctrl, bool alt) :
            Key(key, shift, ctrl, alt)
        {}
    };

    struct KeyDownEvent : public KeyEvent
    {
        KeyDownEvent(char key, bool shift, bool ctrl, bool alt) :
            KeyEvent(key, shift, ctrl, alt)
        {}
    };

    struct KeyUpEvent : public KeyEvent
    {
        KeyUpEvent(char key, bool shift, bool ctrl, bool alt) :
            KeyEvent(key, shift, ctrl, alt)
        {}
    };
};
