#pragma once

#include <cstdlib>
#include <new>
#include <utility>
#include <cstddef>
#include <type_traits>

namespace CustomSTL {

template <typename T>
class ObjectPool {
private:
    union Node {
        Node* next;
    }

    Node* head_;   // points to free list
    Node* end_; // points one past end of storage


public:
    ObjectPool(size_t capacity) {
        size_t object_size = std::max(sizeof(T), sizeof(Node*)); // if T is too small, we add padding so that it can fit a pointer

        size_t bytes_needed = capacity * object_size;
        size_t aligned_bytes_needed = (bytes_needed + alignof(T) - 1) & ~(alignof(T) - 1);

        head_ = static_cast<Node *>(std::aligned_alloc(alignof(T), aligned_bytes_needed));
        if (head_ == nullptr) {
            throw std::bad_alloc;
        }

        // construct the free list
        Node* curr = head_;
        for (size_t i = 1; i < capacity; ++i) {
            curr->next = reinterpret_cast<Node *>(static_cast<char *>(curr) + object_size);
            curr = curr->next;
        }

        curr->next = nullptr;
    }

    // User needs to enforce that they release all objects before object pool is destructed
    // else there is a potential memory leak
    ~ObjectPool() {
        std::free(head_);
    }

    template <typename... Args>
    T* acquire(Args&&... args) {
        if (head_ == nullptr) {
            return nullptr;
        }

        Node* next_head = head_->next;
        T* obj = reinterpret_cast<T*>(head_);
        new (obj) T(std::forward<Args>(args)...);

        head_ = next_head;
        return obj;
    }

    void release(T* ptr) {
        ptr->~T();
        
        Node* new_head = reinterpret_cast<Node*>(ptr);
        new_head->next = head_;
        head_ = new_head;
    }

};

}