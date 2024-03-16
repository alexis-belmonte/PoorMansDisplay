#pragma once

#include "pmd/Event.hpp"

#include <functional>
#include <queue>
#include <memory>
#include <mutex>

#include <cstdint>

namespace PMD
{
    class EventQueue
    {
    public:
        template<typename T,
                 typename = std::enable_if_t<std::is_base_of<Event, T>::value>>
        class Handler
        {
        public:
            Handler(std::function<void(const T &)> handler) :
                _handler(handler)
            {}

            void operator()(const T &event) const
            {
                this->_handler(event);
            }

        protected:
            std::function<void(const T &)> _handler;
        };

    public:
        template<typename T,
                 typename ...Args,
                 typename = std::enable_if_t<std::is_base_of<Event, T>::value>>
        inline void push(Args... eventArgs)
        {
            std::lock_guard<std::mutex> lock(this->_queueMutex);

            this->_queue.emplace(std::make_unique<T>(eventArgs...));
        }

        ::size_t size() const;

    protected:
        template <typename T, typename ...NextArgs>
        inline void pollOne(const std::unique_ptr<Event> &event, Handler<T> &handler, NextArgs &&...args)
        {
            if (T *typedEvent = dynamic_cast<T *>(event.get()))
                handler(*typedEvent);
            if constexpr (sizeof...(NextArgs) > 0)
                this->pollOne(event, args...);
        }

    public:
        template <typename T, typename ...NextArgs>
        inline void poll(Handler<T> &&handler, NextArgs &&...args)
        {
            std::lock_guard<std::mutex> lock(this->_queueMutex);

            while (!this->_queue.empty()) {
                const std::unique_ptr<Event> &event = this->_queue.front();

                this->pollOne(event, handler, args...);
                this->_queue.pop();
            }
        }

    protected:
        std::mutex _queueMutex;
        std::queue<std::unique_ptr<Event>> _queue;
    };
};
