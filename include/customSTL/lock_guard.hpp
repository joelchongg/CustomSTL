#ifndef CUSTOM_STL_LOCK_GUARD_HPP
#define CUSTOM_STL_LOCK_GUARD_HPP

#include <mutex>

namespace CustomSTL {
    template <typename mutex_t>
    class lock_guard {
    public:
        explicit lock_guard(mutex_t& m)
            : mutex_ { m }
        {
            mutex_.lock();
        }

        lock_guard(mutex_t& m, std::adopt_lock_t)
            : mutex_ { m }
        { }

        ~lock_guard() {
            mutex_.unlock();
        }
        
        lock_guard(const lock_guard&) = delete;
        lock_guard& operator=(const lock_guard&) = delete;

    private:
        mutex_t& mutex_;
    };
}

#endif