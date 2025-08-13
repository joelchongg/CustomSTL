# Weak Pointer
Below highlights some of the key points learnt while developing `weak_ptr` as well as reasoning behind the design choices.

# `ControlBlock` Pointer Member
A `weak_ptr` object can extend the lifetime of an object even when there is no longer any `shared_ptr` object pointing to the shared object. Hence, we require `weak_ptr` to store a pointer to the control block, so that it can still access the shared object, and allow its deletion when the weak_count hits zero.