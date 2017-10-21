#include <nova/io.h>

using namespace nova;

template<class CharT>
class string_view_buffer_provider
{
public:
    typedef in_buffer_provider            category;

    typedef CharT                         char_type;
    typedef std::basic_string<CharT>      string_type;
    typedef std::basic_string_view<CharT> string_view_type;

    explicit string_view_buffer_provider(string_view_type str) : _str{str} {}

    std::pair<const char_type*, std::size_t> get_in_buffer()
    {
        if (_buffer_provided) return {nullptr, 0};
        _buffer_provided = true;
        return {_str.data(), static_cast<std::size_t>(_str.size())};
    }

    inline void reset() { _buffer_provided = false; }
    inline string_view_type view() { return _str; }
private:
    string_view_type _str;
    bool _buffer_provided = false;
};

int main()
{
    instream<string_view_buffer_provider<char>> in{"123 456"};
    int i1, i2;
    in >> i1 >> i2;
    std::cout << i1 << " " << i2 << std::endl;
    return 0;
}
