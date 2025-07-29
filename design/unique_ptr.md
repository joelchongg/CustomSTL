# Unique Pointer
Below highlights some of the key points learnt while developing `unique_ptr` as well as reasoning behind the design choices.

### EBO Inheritance
The size of `unique_ptr` is usually the same size as a raw pointer, unless a customer deleter is present. Hence, we need a way to store a custom deleter (if present) such that we do not incur overhead should the default deleter be used.
&nbsp;
If we store the custom deleter as a member variable, we will **always** incur overhead. Hence, we leverage **Empty Base Optimization** to avoid this, by making `unique_ptr` privately inherit from a `Deleter` class. This way, if the `Deleter` is empty, the compiler will optimize away its storage entirely - resulting in `unique_ptr` to have its size equal to a raw pointer!

