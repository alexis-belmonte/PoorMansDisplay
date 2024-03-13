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
        static constexpr const char SET_CURSOR_HOME[]          = "\e[H";
        static constexpr const char GET_CURSOR_POSITION[]      = "\e[6n";
        static constexpr const char STORE_CURSOR_POSITION[]    = "\e[s";
        static constexpr const char RESTORE_CURSOR_POSITION[]  = "\e[u";

        static constexpr const char HIDE_CURSOR_CARET[]        = "\e[?25l";
        static constexpr const char SHOW_CURSOR_CARET[]        = "\e[?25h";

        static constexpr const char FLIP_SCREEN_NORMAL[]       = "\e[?1049h";
        static constexpr const char FLIP_SCREEN_ALTERNATE[]    = "\e[?1049l";

        static constexpr const char CLEAR_SCREEN[]             = "\e[2J";
        static constexpr const char RESET_TERMINAL[]           = "\ec";

        static constexpr const char SET_RGB_BGCOLOR[]          = "\e[48;2;{};{};{}m";
        static constexpr const char SET_RGB_FGCOLOR[]          = "\e[38;2;{};{};{}m";
    };
};
