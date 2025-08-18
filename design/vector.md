# Vector
Below highlights some of the key points learnt while developing `vector` as well as reasoning behind the design choices.

# `Vector<bool>` class specialization
As a boolean only requires 1 bit to handle its states, it is wasteful for vector to use any data type (e.g `int`, `char`, etc.) as the extra memory allocated is not used. Hence, in order to reduce memory usage, we would create a partial template specialization for the `vector<bool>` so as to ensure that each boolean only utilizes one bit.

# `Vector` Copy Constructor
When implementing the copy constructor of `vector`, I found myself stuck on whether to make the new `vector` object's capacity the same as the capacity of the `vector` object that is passed. C++ states that the capacity must be at least enough to hold all the elements, but whether it is bigger is implementation-defined. Hence, my decision was to make it such that the capacity of the new `vector` object is just enough to hold all the elements. My reason for doing so was mainly to optimize space usage. However, this may prove to be detrimental to performance, especially if we were to add elements thereafter, which would immediately cause resizing to occur. However, I believe that there are workarounds to this, such as reserving the vector's capacity prior to copying. Hence, I would still stick to my decision of initializing the `vector` object's capacity to be the same as its size. (If anyone has any better suggestions do let me know!)
&nbsp;
Additionally, in order to optimize for storing trivial types, I utilize `memcpy` instead of using a for loop for copying elements. This makes it much faster as no constructor calls are required for initializing the elements. 