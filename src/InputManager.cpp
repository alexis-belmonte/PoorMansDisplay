#include "pmd/InputManager.hpp"

#include <cstring>

namespace PMD
{
    InputManager::InputManager(EventQueue &eventQueue) :
        _eventQueue(eventQueue)
    {}

    void InputManager::pushKey(char key, bool shift, bool ctrl, bool alt)
    {
        Key keyRepr = {key, shift, ctrl, alt};

        if (!this->_keyStatus.contains(keyRepr)) {
            this->_keyStatus[keyRepr] = {KeyState::Phase::DOWN, std::chrono::steady_clock::now()};
            this->_eventQueue.push<KeyDownEvent>(key, shift, ctrl, alt);
            return;
        }

        KeyState &state = this->_keyStatus.at(keyRepr);
        auto now = std::chrono::steady_clock::now();

        switch (state.phase) {
            case KeyState::Phase::DOWN: {
                if (now - state.lastEventTimestamp <= InputManager::KEY_DOWN_TIMEOUT) {
                    this->_eventQueue.push<KeyUpEvent>(key, shift, ctrl, alt);
                    this->_eventQueue.push<KeyDownEvent>(key, shift, ctrl, alt);
                } else
                    state.phase = KeyState::Phase::REPEAT;
                break;
            }

            case KeyState::Phase::REPEAT: {
                if (now - state.lastEventTimestamp > InputManager::KEY_REPEAT_DELAY) {
                    this->_eventQueue.push<KeyUpEvent>(key, shift, ctrl, alt);
                    this->_eventQueue.push<KeyDownEvent>(key, shift, ctrl, alt);

                    state.phase = KeyState::Phase::DOWN;
                }

                break;
            }
        }

        state.lastEventTimestamp = now;
    }

    void InputManager::feed(const std::string &feedback)
    {
        auto now = std::chrono::steady_clock::now();

        std::erase_if(this->_keyStatus, [this, &now](const auto &pair) {
            const auto &[key, state] = pair;

            if (now - state.lastEventTimestamp < (
                    state.phase == KeyState::Phase::DOWN ?
                        InputManager::KEY_DOWN_TIMEOUT :
                        InputManager::KEY_REPEAT_DELAY
                ))
                return false;
                
            this->_eventQueue.push<KeyUpEvent>(key.key, key.shift, key.ctrl, key.alt);

            return false;
        });

        for (::size_t i = 0; i < feedback.size();) {
            if (feedback[i] != '\x1b') {
                char key = feedback[i];
                bool shift = std::isupper(key);

                if (shift)
                    key = std::tolower(key);

                this->pushKey(key, shift, false, false);
                i++;
                continue;
            }

            // TODO: Implement this
            return;
        }
    }
};
