# nova::stream

It is very simple C++ single-include-file library which helps to write C++ in/out streams easily.

__nova::stream__ is similar to [boost::iostreams](http://www.boost.org/doc/libs/1_65_1/libs/iostreams/doc/index.html).
If you're wondering why I had to write my own streams library rather than
using excellent __boost::iostreams__ I answer it in
["Why new stream library"](https://github.com/novalexei/nstream/wiki/Why-new-stream-library)

Just to demonstrate how easy it is to write your own streams here is a code example:

```cpp
#include <nova/io.h>

using namespace nova;

template<typename CharT>
class string_sink
{
public:
    typedef sink                          category;

    typedef CharT                         char_type;
    typedef std::basic_string<CharT>      string_type;
    typedef std::basic_string_view<CharT> string_view_type;

    string_sink() : _buffer{} {}
    explicit string_sink(string_type str) : _buffer{str} {}

    std::streamsize write(const CharT* s, std::streamsize n)
    {
        _buffer.append(s, n);
        return n;
    }
    void flush() { }

    string_view_type view() const { return string_view_type{_buffer}; }
private:
    string_type _buffer;
};

template<typename CharT>
class string_ref_source
{
public:
    typedef source                   category;

    typedef CharT                    char_type;
    typedef std::basic_string<CharT> string_type;

    explicit string_ref_source(const string_type& str) : _buffer{str} {}

    inline std::streamsize read(CharT* s, std::streamsize n)
    {
        if (ptr >= _buffer.size()) return 0;
        std::streamsize toWrite = n < _buffer.size() - ptr ? n : _buffer.size() - ptr;
        std::copy(_buffer.data()+ptr, _buffer.data()+ptr+toWrite, s);
        ptr += toWrite;
        return toWrite;
    }
private:
    const string_type& _buffer;
    typename string_type::size_type ptr = 0;
};

int main()
{
    outstream<string_sink<char>, buffer_16> out;
    out << 123 << ' ' << 456;
    out.flush();
    std::cout << out->view() << std::endl;
    std::string s{"123 456"};

    instream<string_ref_source<char>> in{s};
    int i1, i2;
    in >> i1 >> i2;
    std::cout << i1 << " " << i2 << std::endl;
    return 0;
}
```

__nova::stream__ also has an exceptional [performance](https://github.com/novalexei/nstream/wiki/Performance)

For details on how to use __nova::stream__ check out the
[Tutorial](https://github.com/novalexei/nstream/wiki/nstream-Tutorial)