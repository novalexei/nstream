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

    std::streamsize write(const char_type* s, std::streamsize n)
    {
        _buffer.append(s, n);
        return n;
    }
    void flush() { }

    string_view_type view() const { return string_view_type{_buffer}; }
private:
    string_type _buffer;
};

int main()
{
    outstream<string_sink<char>, buffer_16> out;
    out << 123 << ' ' << 456;
    out.flush();
    std::cout << out->view() << std::endl;
    return 0;
}
