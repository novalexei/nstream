#include <nova/io.h>

using namespace nova;

template<typename CharT, typename Traits = std::char_traits<CharT>>
class buffer_sink
{
public:
    typedef CharT                         char_type;
    typedef std::basic_string_view<CharT> string_view_type;
    typedef out_buffer_provider           category;

    explicit buffer_sink(std::size_t initial_capacity = 16) :
            _buffer{new char_type[initial_capacity]}, _size{0}, _capacity{initial_capacity} {}

    ~buffer_sink() { delete[] _buffer; }

    std::pair<char_type*, std::size_t> get_out_buffer()
    {
        if (_provided_up_to < _capacity)
        {
            _size = _provided_up_to;
            _provided_up_to = _capacity;
            return {_buffer + _size, _capacity - _size};
        }
        char_type* old_buf = _buffer;
        std::size_t old_capacity = _capacity;
        _capacity *= 2;
        _buffer = new char_type[_capacity];
        std::copy(old_buf, old_buf+old_capacity, _buffer);
        delete[] old_buf;
        _size = _provided_up_to;
        _provided_up_to = _capacity;
        return {_buffer + old_capacity, _capacity - old_capacity};
    }

    void flush(int size) { _size += size; }

    string_view_type view() const { return string_view_type{_buffer, _size}; }
private:
    char_type* _buffer;
    std::size_t _size;
    std::size_t _capacity;
    std::size_t _provided_up_to = 0;
};

template<typename CharT>
class small_buffer_sink
{
    inline static constexpr std::size_t SMALL_BUFFER_CAPACITY = 16; // sizeof(int64_t) / sizeof(CharT);
public:
    typedef out_buffer_provider           category;

    typedef CharT                         char_type;
    typedef std::basic_string_view<CharT> string_view_type;

    small_buffer_sink() = default;

    ~small_buffer_sink() { if (_capacity > SMALL_BUFFER_CAPACITY) delete[] _buffer; }

    std::pair<CharT*, std::size_t> get_out_buffer()
    {
        if (_provided_starting < 0)
        {
            _provided_starting = 0;
            return {_buffer, _capacity};
        }
        CharT* old_buf = _buffer;
        std::size_t old_capacity = _capacity;
        _capacity *= 2;
        _buffer = new CharT[_capacity];
        std::copy(old_buf, old_buf + old_capacity, _buffer);
        if (old_capacity > SMALL_BUFFER_CAPACITY) delete[] old_buf;
        _size = old_capacity;
        _provided_starting = static_cast<int>(old_capacity);
        return {_buffer + old_capacity, _capacity - old_capacity};
    }

    void flush(int size) { _size += size; }

    string_view_type view() const { return string_view_type{_buffer, _size}; }

private:
    CharT _local[SMALL_BUFFER_CAPACITY]{};
    CharT* _buffer = _local;
    std::size_t _size = 0;
    std::size_t _capacity = SMALL_BUFFER_CAPACITY;
    int _provided_starting = -1;
};

int main()
{
    outstream<buffer_sink<char>> out;
    out << 123 << ' ' << 456;
    out.flush();
    std::cout << out->view() << std::endl;
    return 0;
}
