#pragma once

#include <atomic>
#include <immintrin.h>
#include <stdexcept>
#include <sys/mman.h>
#include <type_traits>

namespace CustomSTL {

template <typename T, std::size_t Capacity>
class SeqLockRingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");
    static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");    
public:
    SeqLockRingBuffer()
        : write_head_ { 0 } {
        void* addr = mmap(NULL, Capacity * sizeof(Slot), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, 0, 0);
        if (addr == MAP_FAILED) [[unlikely]] {
            throw std::runtime_error("unable to create mmap");
        }

        data_ = static_cast<Slot*>(addr);

        // default construct all slots
        for (int i = 0; i < Capacity; ++i) {
            new (data_ + i) Slot();
        }
    }

    ~SeqLockRingBuffer() {
        if (data_) {
            munmap(data_, Capacity * sizeof(Slot));
        }
    }

    SeqLockRingBuffer(const SeqLockRingBuffer&) = delete;
    SeqLockRingBuffer& operator=(const SeqLockRingBuffer&) = delete; 

    void push(const T& value) {
        uint64_t write_cnt = write_head_.fetch_add(1, std::memory_order_relaxed);
        uint64_t generation = write_cnt >> __builtin_ctzll(Capacity);
        uint64_t slot_idx = write_cnt & (Capacity - 1);

        Slot& slot = data_[slot_idx];
        slot.seq.store((generation << 1) | 1, std::memory_order_release);
        slot.elem = value;      // implement overloads for lvalue / rvalue when necessary
        slot.seq.store((generation << 1) + 2, std::memory_order_release);
    }

    class Reader {
    public:
        explicit Reader(SeqLockRingBuffer& buf) : buf_ { buf } {}

        void read(T& output, bool& overrun) {
            overrun = false;

            while (true) {
                uint64_t slot_idx = read_head_ & (Capacity - 1);
                Slot& slot = buf_.data_[slot_idx];

                uint64_t slot_gen_v1 = slot.seq.load(std::memory_order_acquire);
                uint64_t read_generation = read_head_ >> __builtin_ctzll(Capacity);

                // same expected generation and not currently in a write
                uint64_t expected_seq = (read_generation << 1) + 2;
                if (slot_gen_v1 == expected_seq) {
                    T tmp = slot.elem;

                    std::atomic_thread_fence(std::memory_order_acquire);    // required to prevent T tmp = slot.elem from being reordered below
                    uint64_t slot_gen_v2 = slot.seq.load(std::memory_order_acquire);
                    
                    if (slot_gen_v1 == slot_gen_v2) {
                        output = tmp;
                        ++read_head_;
                        return;    
                    }
                    continue;
                }

                if (slot_gen_v1 > expected_seq) {
                    overrun = true;
                    read_head_ = buf_.write_head_.load(std::memory_order_acquire);
                    continue;
                }

                // on spin, we pause
                _mm_pause();
            }
        }

    private:
        SeqLockRingBuffer& buf_;
        uint64_t read_head_ = 0;
    };

private:
    struct Slot {
        alignas(64) std::atomic<uint64_t> seq{0};
        T elem;
    };

    alignas(64) std::atomic<uint64_t> write_head_;
    Slot* data_ = nullptr;
};

} // namespace CustomSTL