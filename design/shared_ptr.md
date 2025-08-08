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

# `PointerCompatible` Concept
The C++ Standard states a few requirements for a pointer of type `Y` to be stored in a `shared_ptr` of type `T`. As such, to follow good design, I have created a concept called `PointerCompatible` that captures the conditions.

# Relational Operators
One thing I found interesting when the functions implemented for `shared_ptr` was that relational operators are not implemented, despite it being implemented for `unique_ptr`. Well, it turns out that the reasoning is due to multiple `shared_ptr` objects having the possibility of sharing the same object. Hence, this complicates the concept of ordering, especially amongst two `shared_ptr` objects pointing to the same object. On the other hand, since `unique_ptr` has clear pointer ownership, it is easy to establish an ordering. As such, I will also not provide relational operator support, although it is easy to do so for two different `shared_ptr` objects, and only support `operator==`. Although the C++ Standard Library implements `operator<=>`, comparing based on the pointer addresses. With my current knowledge, I feel that there is no point in doing so, and it seems like it is just for completeness. Hence, I will not be implementing `operator<=>` for `shared_ptr`. If you have a good reason do let me know! I will be more than happy to learn.

# `owner_before()`
Some confusion I had with `owner_before()` was that what is the point of a weak ordering? Why does it arbitrarily compare the memory addresses even if a `shared_ptr` object may be created before another? This boils down to being able to use `shared_ptr` as keys in associative containers such as `map` or `set`, where we do not need to enforce a total order, just any order. As such I wondered why not make `owner_before()` work such that if a `shared_ptr` object was created before another `shared_ptr` object, even if they point to the same object, `owner_before()` returns true? This results in a total order based on the order of creation. While possible, this may result in performance issues, since now we need to keep track of creation time and store extra metadata. This may also result in more work when trying to make it work properly for concurrency, adding **runtime overhead and complexity**. Since containers just need a **consistent ordering**, it makes sense that a weak ordering will do just fine. For now, my `owner_before()` function will also employ a weak ordering. However, as I gain more experience in C++ and being able to consider more tradeoffs, I might change it to what I originally intended, depending on its benefits.

