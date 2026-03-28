#pragma once

#include <atomic>
#include <cstddef>
#include <iostream>
#include <thread>
#include <vector>

namespace CustomSTL {

template <typename T, size_t Capacity>
class SPSCQueue {
public:
    bool push(T& input) {
        size_t tail_idx = tail_.load(std::memory_order_relaxed);
        if ((tail_idx + 1) % (Capacity + 1) == head_.load(std::memory_order_relaxed)) {
            return false;
        }

        data_[tail_idx] = input;
        tail_.store((tail_idx + 1) % (Capacity + 1), std::memory_order_release);
        return true;
    }

    bool push(T&& input) {
        size_t tail_idx = tail_.load(std::memory_order_relaxed);
        if ((tail_idx + 1) % (Capacity + 1) == head_.load(std::memory_order_relaxed)) {
            return false;
        }

        data_[tail_idx] = std::move(input);
        tail_.store((tail_idx + 1) % (Capacity + 1), std::memory_order_release);
        return true;
    }

    bool pop(T& output) {
        size_t head_idx = head_.load(std::memory_order_relaxed);
        if (head_idx == tail_.load(std::memory_order_acquire)) {
            return false;
        }

        output = data_[head_idx];
        head_.store((head_idx + 1) % (Capacity + 1), std::memory_order_release);
        return true;
    }

private:
    alignas(64) std::atomic<size_t> head_ { 0 };
    alignas(64) std::atomic<size_t> tail_ { 0 };
    alignas(64) T data_[Capacity + 1];

};

}