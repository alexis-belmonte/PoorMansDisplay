#include "pmd/EventQueue.hpp"

namespace PMD
{
    ::size_t EventQueue::size() const
    {
        return this->_queue.size();
    }
};
