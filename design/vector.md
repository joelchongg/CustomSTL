# Vector
Below highlights some of the key points learnt while developing `vector` as well as reasoning behind the design choices.

# `Vector<bool>` class specialization
As a boolean only requires 1 bit to handle its states, it is wasteful for vector to use any data type (e.g `int`, `char`, etc.) as the extra memory allocated is not used. Hence, in order to reduce memory usage, we would create a partial template specialization for the `vector<bool>` so as to ensure that each boolean only utilizes one bit.