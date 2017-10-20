# nova::stream

Very simple C++ single-include-file framework which helps to write C++ input/output streams easily.

__nova::stream__ is similar to [boost::iostreams](http://www.boost.org/doc/libs/1_65_1/libs/iostreams/doc/index.html).
If you're wondering why I had to write my own streams library rather than
using excellent __boost::iostreams__ I answer it in
["Why new stream library"](https://github.com/novalexei/nstream/wiki/Why-new-stream-library)

The features of __nova::stream__

- Very small. Single include file. No dependencies.
- Works with C++17, but should also compile with C++14 and likely with C++11
- Supports input stream, output streams and devices shared between input and output streams
- Allows to provide a buffer directly to input or output stream for performance reasons
 
For details on how to write C++ streams with __nova::stream__ check out the
[Tutorial](https://github.com/novalexei/nstream/wiki/nstream-Tutorial)

__nova::stream__ also has an exceptional [performance](https://github.com/novalexei/nstream/wiki/nstream-Performance)
