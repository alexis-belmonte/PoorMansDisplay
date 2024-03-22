#include "pmd/InputManager.hpp"

#include <cstring>

namespace PMD
{
    InputManager::InputManager(EventQueue &eventQueue) :
        _eventQueue(eventQueue)
    {}

    void InputManager::handleKey(char key, bool shift, bool ctrl, bool alt, std::chrono::steady_clock::time_point &now)
    {
        Key keyRepr = {key, shift, ctrl, alt};

        if (!this->_keyStatus.contains(keyRepr)) {
            this->_keyStatus[keyRepr] = {KeyState::Phase::DOWN, std::chrono::steady_clock::now()};
            this->_eventQueue.push<KeyDownEvent>(key, shift, ctrl, alt);
            return;
        }

        KeyState &state = this->_keyStatus.at(keyRepr);

        switch (state.phase) {
            case KeyState::Phase::DOWN: {
                if (now - state.lastEventTimestamp <= InputManager::KEY_DOWN_TIMEOUT)
                    state.phase = KeyState::Phase::REPEAT;
                break;
            }

            case KeyState::Phase::REPEAT: {
                break;
            }
        }

        state.lastEventTimestamp = now;
    }

    void InputManager::feed(const std::string &feedback)
    {
        auto now = std::chrono::steady_clock::now();

        for (::size_t i = 0; i < feedback.size();) {
            if (feedback[i] != '\x1b') {
                char key = feedback[i];
                bool shift = std::isupper(key);

                if (shift)
                    key = std::tolower(key);

                this->handleKey(key, shift, false, false, now);
                i++;
                continue;
            }

            // TODO: Implement complex escape sequences
            return;
        }

        std::erase_if(this->_keyStatus, [this, &now](const auto &pair) {
            const auto &[key, state] = pair;

            if (now - state.lastEventTimestamp <= (
                    state.phase == KeyState::Phase::DOWN ?
                        InputManager::KEY_DOWN_TIMEOUT :
                        InputManager::KEY_REPEAT_DELAY
                ))
                return false;

            this->_eventQueue.push<KeyUpEvent>(key.key, key.shift, key.ctrl, key.alt);
            return true;
        });
    }
};
