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