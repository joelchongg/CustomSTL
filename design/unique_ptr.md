# Unique Pointer
Below highlights some of the key points learnt while developing `unique_ptr` as well as reasoning behind the design choices.

### EBO Storage Methods
The size of `unique_ptr` is usually equal to the size of a raw pointer when using the default deleter. This is achieved via **Empty Base Optimization (EBO)** - a compiler optimization that eliminates the space overhead of empty base classes. To preserve this property, we need a design that stores the deleter in a way that avoids overhead when it is empty.
&nbsp;
There are several ways to manage deleter storage, each with its own tradeoffs:
1. **Store the deleter as a data member**
    The most simplest approach would be to store the deleter as a member. However, it does not utilize EBO, and incurs a space cost even when the deleter is stateless. This violates the STL principle of **zero-overhead abstraction**, which states that abstractions should not cost more than equivalent handwritten code.
2. **Privately inherit from the deleter**
    This is the strategy that is used in GCC's STL implementation.
    This enables EBO, as empty base classes do not occupy storage, whilst maintaining simplicity. However, some may feel that this approach introduces poor encapsulation: if the deleter is stateful or not intended for inheritance, this may break class invariants or introduct subtle issues (e.g slicing or unexpected method exposure)
3. **Use of a `CompressedPair<S, T>` helper class with conditional EBO**
    This is the approach that Clang has taken in its STL implementation.
    This design separates optimization concerns from the main `unique_ptr` logic. The `CompressedPair` class conditionally inherits from either `S` or `T` based on whether it is empty, and stores the other as a regular member. This way, we can leverage on EBO to optimize the space usage of `unique_ptr`. Furthermore, the `CompressedPair` class can also be reused for other STL constructs (e.g., `tuple`, `optional`, `function`). However, some may feel that this adds abstraction complexity.

    For now, I have chosen to leverage EBO through private inheritance. It is simple and direct, and guarantees minimal space overhead for the common case. Furthermore, it is easier for people to understand and implement when focusing on `unique_ptr` alone. However, I may change to create and use a reusable `CompressedPair` utility class to handle more complex cases or multiple EBO candidates in other containers.

### Substitution Failure Is Not An Error (SFINAE)
Whilst exploring how to implement the array specialization for `unique_ptr`, I stumbled upon the `reset()` function overload, where we can reset `unique_ptr` with a pointer to another class, so long as the class is the same type or fulfils certain requirements.
In order to check for this, we need to leverage SFINAE, such that the compiler knows when to use the function overload, and when to discard it from consideration. This is enabled by utilizing the `requires` or `std::enable_if` in the template parameter list, where we check if the pointer argument fulfils the requirements. I have chosen to use `requires` since it is cleaner, more readable and less error-prone than `std::enable_if`. More details can be found in `reset()` for the array specialization of `unique_ptr`.

### Deleter Overload & `get_deleter()` Implementation
In the implementation, you (as well as myself) may wonder why is there an overload for `get_deleter()`. For those that are new, this is to decouple **const or non-const** `unique_ptr`. If we call `get_deleter()` on a **non-const** `unique_ptr`, we will get mutable reference to the deleter. Similarly for a **const** `unique_ptr`.

Additionally, you (as well as myself) may wonder how would we obtain a reference to the Deleter object, since I had chosen to leverage EBO earlier through private inheritance instead of storing it as a `CompressedPair`. In this case, we can simply use a cast to refer to the base class.

# Constexpr/ Noexcept optimizations
In order to increase compile-time usability and optimize performance, I have added `constexpr` and `noexcept` wherever possible (if there are any errors do let me know!). By making functions `constexpr`, it tells the compiler that the function can be computed at compile time, improving constant propagation and folding. This decreases the runtime cost. As for noexcept, it tells the compiler that the function will never throw an exception, allowing it to generate more optimized code since it does not need to generate stack unwinding or exception-handling metadata.
&nbsp;
As these optimizations will be included throughout any other containers/classes created in the future, I will no longer include the explanation for the usage of these keywords in subsequent designs.

# Constructors
The STL implements numerous constructors for different use cases of `unique_ptr`. This may be confusing as to how the implementation is being done, and how SFINAE is leveraged in order to determine which constructors can be used.

The implementation for additional constructors for the specialized template will be done eventually.

# Small Object Optimization (SOO)
While the standard `unique_ptr` in the C++ STL does not implement Small Object Optimization (SOO), I have decided to try to design and implement a custom smart pointer called `small_unique_ptr` that incorporates SOO to leverage its benefits.

**What is SOO?**
SOO is an optimization technique where small objects are stored directly inside the smart pointer itself (typically on the stack or within the smart pointer’s internal storage) instead of being dynamically allocated on the heap. This reduces the overhead of heap allocation and deallocation, which can be significant when managing many small objects.

**Why SOO?**
Automatic (stack or internal) storage is generally faster than dynamic (heap) storage due to reduced allocation overhead and better cache locality. By storing small objects inline, `small_unique_ptr` reduces pointer indirection and eliminates many calls to new and delete, improving performance especially in scenarios involving frequent creation and destruction of small objects.

**Features of small_unique_ptr:**
- Stores small objects inline if their size fits within a predefined internal buffer size.
- Falls back to dynamic heap allocation for larger objects.
- Maintains unique ownership semantics similar to unique_ptr.
- Provides standard smart pointer interface (operator*, operator->, get(), release(), reset(), etc.).
- Supports move semantics while disabling copy semantics.
- Balances performance optimization with the safety and convenience of RAII.

By combining SOO with the well-understood and safe interface of `unique_ptr`, `small_unique_ptr` offers an efficient way to manage dynamically or automatically stored objects with a single abstraction.

**How do we choose the buffer size?**
You (or as did I) may wonder:  How do we decide on the buffer size so that storing objects on the stack yields the greatest performance benefit compared to heap allocation? If the buffer size is **too small**, many objects won’t fit inline and will still be heap-allocated, making `small_unique_ptr` almost as costly as a regular `unique_ptr`. If the buffer size is **too large**, every `small_unique_ptr` instance becomes big, increasing memory usage and potentially reducing cache efficiency. The answer is that it depends!

&nbsp;
The optimal buffer size depends heavily on the requirements of our systems, the typical sizes of objects we plan to manage as well as our performance vs memory tradeoffs that we are willing to accept. Because of this variability, I designed `small_unique_ptr` to take a non-template parameter `BufferSize`, allowing us to customize the inline buffer size. This flexibility lets us benchmark and tune the buffer size for our specific use case, measuring the real-world impact on performance and memory.
&nbsp;
One important note is that the buffer must be properly aligned to hold any object safely without undefined behavior. Misalignment can cause crashes or degrade performance, due to **data structure alignment** requirements enforced by the hardware and compiler.

`small_unique_ptr` is designed to fallback to heap allocation should the user provide an object whose size is bigger than the buffer size. Hence, it is on the user to check if the object is constructed inline in case of any performance issues. Other possibilities could be throwing an exception if the object is too big. But this would result in there being a runtime exception. Hence, I may eventually switch to checking at compile time if that is possible, so that the error is found at compile time if the size is too big, allowing the user to use `unique_ptr` instead for better efficiency (this may violate noexcept as an exception is thrown, preventing noexcept compile optimizations).

### Unsupported / Future Additions for `small_unique_ptr`
- I have yet to implement the `Deleter` template parameter and associated methods.
- `swap()` method in order to swap 2 `small_unique_ptr` objects.
- Query member functions (e.g `is_constructed_inline`, `stack_buffer_size`) which may help users check where objects are allocated.
- `release()` is not implemented yet.
- Other constructors/ methods that are present in the original `unique_ptr` class that are not present in `small_unique_ptr`.