#pragma once

#include <string>

namespace PMD
{
    class EscapeSequence
    {
    public:
        static constexpr const char ENABLE_MOUSE_CAPTURE[]     = "\e[?1000h";
        static constexpr const char DISABLE_MOUSE_CAPTURE[]    = "\e[?1000l";

        static constexpr const char SET_CURSOR_POSITION[]      = "\e[{};{}H";
        static constexpr const char GET_CURSOR_POSITION[]      = "\e[6n";
        static constexpr const char STORE_CURSOR_POSITION[]    = "\e[s";
        static constexpr const char RESTORE_CURSOR_POSITION[]  = "\e[u";
    };
};
