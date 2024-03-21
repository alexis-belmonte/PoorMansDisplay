#pragma once

#include <compare>
#include <functional>

namespace PMD
{
    struct Key
    {
        Key(char key, bool shift, bool ctrl, bool alt) :
            key(key),
            shift(shift),
            ctrl(ctrl),
            alt(alt)
        {}

        char key;
        bool shift;
        bool ctrl;
        bool alt;

        auto operator<=>(const Key &) const = default;
    };
};

template<>
struct std::hash<PMD::Key>
{
    ::size_t operator()(const PMD::Key& k) const
    {
        return (std::hash<char>()(k.key)
             ^ (std::hash<bool>()(k.shift) << 8)
             ^ (std::hash<bool>()(k.ctrl) << 9)
             ^ (std::hash<bool>()(k.alt) << 10));
    }
};
