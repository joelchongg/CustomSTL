# Shared Pointer
Below highlights some of the key points learnt while developing `shared_ptr` as well as reasoning behind the design choices.

### `ControlBlock` Class
The `shared_ptr` class usually stores a pointer pointing to the object as well as a pointer to a shared control block. As such, I have developed a basic `ControlBlock` class in order to mimic this. The `ControlBlock` class stores a few main items:
- Either a pointer to the managed object or the managed object itself
- `strong_count` - indicates how many `shared_ptr` objects own the managed object
- `weak_count` - indicates how many `weak_ptr` objects that refer to the managed object
- Deleter (Not implemented yet)
- Allocator (Not implemented yet)

I initially thought it made sense to use unsigned integers for `strong_count` and `weak_count` since these counts can never be negative. However, I soon realized that just using unsigned types isn’t enough in a multithreaded context. To ensure thread safety, these counts need to be wrapped in `std::atomic`. This is because multiple threads might increment or decrement the counts simultaneously, and without atomic operations, it could lead to data races or incorrect counts. Using `std::atomic` guarantees that all updates happen safely and consistently across threads, which is crucial for managing shared ownership correctly.

However, I will hold off on implementing `std::atomic` until I learn more about making everything thread-safe and learning more about `std::atomic`.