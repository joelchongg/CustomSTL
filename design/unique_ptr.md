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