#pragma once

#include "pmd/EventQueue.hpp"
#include "pmd/Key.hpp"

#include <unordered_map>
#include <chrono>

namespace PMD
{
    struct KeyState
    {
        enum class Phase
        {
            DOWN,
            REPEAT
        } phase;
        std::chrono::time_point<std::chrono::steady_clock> lastEventTimestamp;
    };

    class InputManager
    {
    public:
        static constexpr std::chrono::milliseconds KEY_DOWN_TIMEOUT{250};
        static constexpr std::chrono::milliseconds KEY_REPEAT_DELAY{50};

    public:
        InputManager(EventQueue &eventQueue);

    protected:
        void pushKey(char key, bool shift, bool ctrl, bool alt);

    public:
        void feed(const std::string &feedback);

    protected:
        EventQueue &_eventQueue;

        std::unordered_map<Key, KeyState> _keyStatus;
    };
};
