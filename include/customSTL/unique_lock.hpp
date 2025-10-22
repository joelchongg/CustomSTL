#ifndef CUSTOM_STL_UNIQUE_LOCK_HPP
#define CUSTOM_STL_UNIQUE_LOCK_HPP

#include <concepts>

namespace CustomSTL {
    template <typename Mutex>
    class unique_lock {
    public:
        using mutex_type = Mutex;

        unique_lock() noexcept
            : mt_ { nullptr }
            , flag_ { false }
        { }

        explicit unique_lock(mutex_type& m)
        : mt_ { &m }
        , flag_ { false }
        { 
            mt_->lock();
            flag_ = true;
        }
        
        unique_lock(unique_lock&& other) noexcept
            : mt_ { std::exchange(other.mt_, nullptr) }
            , flag_ { std::exchange(other.flag_, false) }
        { }

        unique_lock(mutex_type& m, std::defer_lock_t t) noexcept
            : mt_ { &m }
            , flag_ { false }
        { }

        unique_lock(mutex_type& m, std::try_to_lock_t t)
            : mt_ { &m }
            , flag_ { false }
        {
            if (mt_.try_lock()) {
                flag_ = true;
            }
        }

        unique_lock(mutex_type& m, std::adopt_lock_t t)
            : mt_ { &m }
            , flag_ { true }
        { }

        template <typename Rep, typename Period>
        unique_lock(mutex_type& m, const std::chrono::duration<Rep, Period>& timeout_duration)
            : mt_ { &m }
            , flag_ { false }
        {
            if (mt_.try_lock_for(timeout_duration)) {
                flag_ = true;
            }
        }

        template <typename Clock, typename Duration>
        unique_lock(mutex_type& m, const std::chrono::time_point<Clock, Duration>& timeout_time)
            : mt_ { &m }
            , flag_ { false }
        {
            if (mt_.try_lock_until(timeout_time)) {
                flag_ = true;
            }
        }

        ~unique_lock() {
            if (flag_) {
                mt_->unlock();
                flag_ = false;
            }
        }

        unique_lock(const unique_lock&) = delete;
        unique_lock& operator=(const unique_lock&) = delete;

        unique_lock& operator=(unique_lock&& other) {
            if (this == &other) {
                return *this;
            }

            if (flag_) {
                mt_->unlock();
            }

            mt_ = std::exchange(other.mt_, nullptr);
            flag_ = std::exchange(other.flag_, false);

            return *this;
        }

        void lock() {
            if (mt_ == nullptr) {
                throw std::system_error(std::errc::operation_not_permitted());
            } else if (flag_) {
                throw std::system_error(std::errc::resource_deadlock_would_occur());
            }

            mt_->lock();
            flag_ = true;
        }

        void unlock() {
            if (mt_ == nullptr || !flag_) {
                throw std::system_error(std::errc::operation_not_permitted());
            }

            mt_->unlock();
            flag_ = false;
        }

        bool try_lock() requires std::lockable<mutex_type> {
            if (mt_ == nullptr) {
                throw std::system_error(std::errc::operation_not_permitted());
            } else if (flag_) {
                throw std::system_error(std::errc::resource_deadlock_would_occur());
            }

            if (mt_->try_lock()) {
                flag_ = true;
            }

            return flag_;
        }

        template <typename Rep, typename Period>
        requires std::timed_lockable<mutex_type>
        bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration) {
            if (mt_ == nullptr) {
                throw std::system_error(std::errc::operation_not_permitted());
            } else if (flag_) {
                throw std::system_error(std::errc::resource_deadlock_would_occur());
            }

            if (mt_->try_lock_for(timeout_duration)) {
                flag_ = true;
            }

            return flag_;
        }

        template <typename Clock, typename Duration>
        requires std::timed_lockable<mutex_type>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration>& timeout_time) {
            if (mt_ == nullptr) {
                throw std::system_error(std::errc::operation_not_permitted());
            } else if (flag_) {
                throw std::system_error(std::errc::resource_deadlock_would_occur());
            }

            if (mt_->try_lock_until(timeout_time)) {
                flag_ = true;
            }

            return flag_;
        }

    private:
        Mutex* mt_;
        bool flag_;
    };
}

#endif