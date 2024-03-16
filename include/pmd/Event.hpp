#pragma once

#include "pmd/Vector.hpp"

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
};
