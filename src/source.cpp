#include <nova/io.h>

using namespace nova;

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
    std::string s{"123 456"};
    instream<string_ref_source<char>> in{s};
    int i1, i2;
    in >> i1 >> i2;
    std::cout << i1 << " " << i2 << std::endl;
    return 0;
}
