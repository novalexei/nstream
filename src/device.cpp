#include <nova/io.h>

using namespace nova;

template<typename CharT>
class string_device
{
public:
    typedef CharT                         char_type;
    typedef std::basic_string<CharT>      string_type;
    typedef std::basic_string_view<CharT> string_view_type;

    typedef in_buffer_provider            in_category;
    typedef sink                          out_category;

    string_device() : _buffer{} {}
    explicit string_device(string_type str) : _buffer{std::move(str)} {}
    ~string_device() noexcept = default;

    std::streamsize write(const CharT* s, std::streamsize n)
    {
        _buffer.append(s, n);
        return n;
    }
    void flush() { }

    std::pair<const CharT*, std::size_t> get_in_buffer()
    {
        if (_in_size == _buffer.size()) return {nullptr, 0};
        auto in_start = _in_size;
        _in_size = _buffer.size();
        return {_buffer.data() + in_start, _in_size};
    }

    string_view_type view() const { return string_view_type{_buffer}; }
private:
    string_type _buffer;
    std::size_t _in_size = 0;
};

int main()
{
    string_device<char> device;
    device_instream<string_device<char>> in{device};
    device_outstream<string_device<char>> out{device};
    out << 123 << ' ' << 456;
    int i1, i2;
    in >> i1 >> i2;
    std::cout << device.view() << std::endl;
    std::cout << i1 << ' ' << i2 << std::endl;
    return 0;
}
