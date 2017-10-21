/*
Copyright (c) 2016 Alexei Novakov
https://github.com/novalexei

Distributed under the Boost Software License, Version 1.0.
http://boost.org/LICENSE_1_0.txt
*/
#ifndef NOVA_IO_H
#define NOVA_IO_H

#include <type_traits>
#include <iostream>

/**
 * @file io.h
 * @brief Definitions for custom implementation of <code>std::istream</code>
 * and <code>std::ostream</code> objects.
 *
 * This resembles to the project <b>boost::iostreams</b>, but provides new sink and
 * source - buffer provider, which provides the buffer directly to <code>inbuf</code>
 * and <code>outbuf</code> which eliminates unnecessary copying.
 *
 * @see <a href="http://www.boost.org/doc/libs/1_62_0/libs/iostreams/doc/index.html">
 * <i>boost::iostreams</i></a>,
 */

namespace nova {

/**
 * Template class holder of the buffer size.
 *
 * @tparam BufSize buffer size
 */
template<std::size_t BufSize>
struct buffering
{
    /**
     * Size of buffer as constant expression.
     */
    static constexpr std::size_t buf_size = BufSize;
};

/**
 * Type definition for zero buffer.
 */
typedef buffering<0>    non_buffered;
/**
 * Type definition for 8b buffer.
 */
typedef buffering<8>    buffer_8;
/**
 * Type definition for 16b buffer.
 */
typedef buffering<16>    buffer_16;
/**
 * Type definition for 32b buffer.
 */
typedef buffering<32>    buffer_32;
/**
 * Type definition for 64b buffer.
 */
typedef buffering<64>    buffer_64;
/**
 * Type definition for 128b buffer.
 */
typedef buffering<128>    buffer_128;
/**
 * Type definition for 256b buffer.
 */
typedef buffering<256>    buffer_256;
/**
 * Type definition for 512b buffer.
 */
typedef buffering<512>    buffer_512;
/**
 * Type definition for 1Kb buffer.
 */
typedef buffering<1024> buffer_1k;
/**
 * Type definition for 2Kb buffer.
 */
typedef buffering<2048> buffer_2k;
/**
 * Type definition for 4Kb buffer.
 */
typedef buffering<4096> buffer_4k;
/**
 * Type definition for 8Kb buffer.
 */
typedef buffering<8192> buffer_8k;

/**
 * Sink tag.
 *
 * This tag is used for the regular sinks provided to
 * <code>nova::basic_outstream</code>. The object with this tag
 * is expected to have the following type definitions:
 *
 * ~~~~~{.cpp}
 * typedef <character type> char_type;
 * typedef sink category;
 * ~~~~~
 *
 * and the following methods:
 *
 * ~~~~~{.cpp}
 * std::streamsize write(const CharT* s, std::streamsize n);
 * void flush();
 * ~~~~~
 *
 * Method <code>write</code> will attempt to write <code>n</code> characters
 * from provided buffer <code>s</code> and return the number of bytes
 * successfully written.
 *
 * Method <code>flush</code> will flush the underlying stream or do nothing
 * if the stream cannot be flushed.
 */
struct sink {};
/**
 * Source tag.
 *
 * This tag is used for the regular sources provided to
 * <code>nova::basic_instream</code>. The object with this tag is expected
 * to have the following type definitions:
 *
 * ~~~~~{.cpp}
 * typedef <character type> char_type;
 * typedef source category;
 * ~~~~~
 *
 * and the following method:
 *
 * ~~~~~{.cpp}
 * std::streamsize read(const CharT* s, std::streamsize n);
 * ~~~~~
 *
 * Method <code>read</code> reads from the underlying stream into buffer
 * <code>s</code> up to <code>n</code> characters and returns the actual
 * number of characters read.
 */
struct source {};
/**
 * Sink tag.
 *
 * This tag is used to indicate that this object can be used as buffer
 * provider for both <code>nova::outstream</code>. The object
 * with this tag is expected to have the following type definitions:
 *
 * ~~~~~{.cpp}
 * typedef <character type> char_type;
 * typedef out_buffer_provider category;
 * ~~~~~
 *
 * and the following methods:
 *
 * ~~~~~{.cpp}
 * std::pair<CharT*, std::size_t> get_out_buffer();
 * void flush(std::size_t size);
 * ~~~~~
 *
 * Method <code>get_out_buffer</code> returns <code>std::pair</code>
 * containing buffer for the stream to use and number of characters
 * available in this buffer. If this method returns <code>{nullptr, 0}</code>
 * it means that no more characters can be written into the output buffer.
 *
 * Note that in C++17 this method can also return
 * <code>std::tuple<char_type*, std::size_t></code> or
 * <code>struct {char_type*, std::size_t}</code>.
 *
 * Method <code>flush</code> is called when the stream is either flushed
 * or closed. As the argument it receives the number of characters written
 * to the stream since last call to <code>get_out_buffer</code> or
 * <code>flush</code>.
 */
struct out_buffer_provider {};
/**
 * Source tag.
 *
 * This tag is used to indicate that this object can be used as buffer
 * provider for both <code>servlet::basic_instream</code>. The object
 * with this tag is expected to have the following type definitions:
 *
 * ~~~~~{.cpp}
 * typedef <character type> char_type;
 * typedef in_buffer_provider category;
 * ~~~~~
 *
 * and the following method:
 *
 * ~~~~~{.cpp}
 * std::pair<const CharT*, std::size_t> get_in_buffer();
 * ~~~~~
 *
 * Method <code>get_in_buffer</code> returns <code>std::pair</code>
 * containing buffer for the stream to use and number of characters
 * available in this buffer. This method can (and will) be called multiple
 * times until it returns <code>{nullptr, 0}</code>.
 *
 * Note that in C++17 this method can also return
 * <code>std::tuple<const char_type*, std::size_t></code> or
 * <code>struct {const char_type*, std::size_t}</code>.
 */
struct in_buffer_provider {};

template<typename Sink, typename Buffering, typename Traits, typename Category = void>
class basic_outbuf;

template<typename Sink, typename Buffering, typename Traits, typename Category>
class basic_outbuf : public std::basic_streambuf<typename Sink::char_type, Traits>
{
    typedef std::basic_streambuf<typename Sink::char_type, Traits> _buf_type;
public:
    typedef typename Sink::char_type  char_type;
    typedef Traits                    traits_type;
    typedef typename Traits::int_type int_type;
    typedef typename Traits::pos_type pos_type;
    typedef typename Traits::off_type off_type;

    typedef std::basic_ostream<char_type, traits_type>  _super_type;

    template<class... Args>
    explicit basic_outbuf(Args &&... args) :
            _sink{std::forward<Args>(args)...}, _buffer{new char_type[Buffering::buf_size]}
    {
        _buf_type::setp(_buffer, _buffer + Buffering::buf_size - 1);
    }

    basic_outbuf(const basic_outbuf& other) = delete;
    basic_outbuf(basic_outbuf&& ) = delete;

    ~basic_outbuf() noexcept override = default;

    basic_outbuf& operator=(const basic_outbuf& ) = delete;
    basic_outbuf& operator=(basic_outbuf&& ) = delete;

    Sink& operator*() { return _sink; }
    Sink* operator->() { return &_sink; }

    const Sink& operator*() const { return _sink; }
    const Sink* operator->() const { return &_sink; }

    void reset() { _buf_type::setp(_buffer, _buffer + Buffering::buf_size - 1); }

protected:
    int_type overflow(int_type ch) override
    {
        _buffer[Buffering::buf_size-1] = static_cast<char>(ch);
        _buf_type::setp(_buffer, _buffer + Buffering::buf_size - 1);
        return _sink.write(_buffer, Buffering::buf_size) < Buffering::buf_size ? traits_type::eof() : ch;
    }

    int sync() override
    {
        std::size_t size = _buf_type::pptr() - _buf_type::pbase();
        if (size > 0)
        {
            if (_sink.write(_buffer, static_cast<std::streamsize>(size)) < size) return -1;
        }
        _sink.flush();
        _buf_type::setp(_buffer, _buffer + Buffering::buf_size - 1);
        return 0;
    }

private:
    Sink _sink;
    char_type *_buffer;
};

template<typename Sink, typename Traits>
class basic_outbuf<Sink, non_buffered, Traits,
                   typename std::enable_if_t<std::is_same<typename Sink::category, out_buffer_provider>::value>> :
        public std::basic_streambuf<typename Sink::char_type, Traits>
{
    typedef std::basic_streambuf<typename Sink::char_type, Traits> _buf_type;
public:
    typedef typename Sink::char_type  char_type;
    typedef Traits                    traits_type;
    typedef typename Traits::int_type int_type;
    typedef typename Traits::pos_type pos_type;
    typedef typename Traits::off_type off_type;

    template<class... Args>
    explicit basic_outbuf(Args &&... args) : _sink{std::forward<Args>(args)...} { }

    basic_outbuf(const basic_outbuf& other) = delete;
    basic_outbuf(basic_outbuf&& ) = delete;

    ~basic_outbuf() noexcept override { sync(); }

    basic_outbuf& operator=(const basic_outbuf& ) = delete;
    basic_outbuf& operator=(basic_outbuf&& ) = delete;

    Sink& operator*() { return _sink; }
    Sink* operator->() { return &_sink; }

    const Sink& operator*() const { return _sink; }
    const Sink* operator->() const { return &_sink; }

    void reset() { _buf_type::setp(_buf_type::pbase(), _buf_type::epptr()); }

protected:
    int_type overflow(int_type ch) override
    {
#if __cplusplus > 201700L
        auto [buf, size] = _sink.get_out_buffer();
        if (!buf || size <= 0) return traits_type::eof();
        *buf = ch;
        _buf_type::setp(buf, buf + size);
        _buf_type::pbump(1);
        return ch;
#else
        auto res = _sink.get_out_buffer();
        if (!res.first || res.second <= 0) return traits_type::eof();
        *res.first = ch;
        _buf_type::setp(res.first + 1, res.first + res.second);
        _buf_type::pbump(1);
        return ch;
#endif
    }

    int sync() override
    {
        _sink.flush(_buf_type::pptr() - _buf_type::pbase());
        _buf_type::setp(_buf_type::pptr(), _buf_type::epptr());
        return 0;
    }

private:
    Sink _sink;
};

template<typename Sink, typename Traits, typename Category>
class basic_outbuf<Sink, non_buffered, Traits, Category> : public std::basic_streambuf<typename Sink::char_type, Traits>
{
    typedef std::basic_streambuf<typename Sink::char_type, Traits> _buf_type;
public:
    typedef typename Sink::char_type  char_type;
    typedef Traits                    traits_type;
    typedef typename Traits::int_type int_type;
    typedef typename Traits::pos_type pos_type;
    typedef typename Traits::off_type off_type;

    template <class... Args>
    explicit basic_outbuf(Args&&... args) : _sink{std::forward<Args>(args)...} {}
    ~basic_outbuf() noexcept override = default;

    basic_outbuf(const basic_outbuf& ) = default;
    basic_outbuf(basic_outbuf&& ) noexcept = default;

    basic_outbuf& operator=(const basic_outbuf& ) = default;
    basic_outbuf& operator=(basic_outbuf&& ) noexcept = default;

    void reset() { }

    Sink& operator*() { return _sink; }
    Sink* operator->() { return &_sink; }

    const Sink& operator*() const { return _sink; }
    const Sink* operator->() const { return &_sink; }

protected:
    int_type overflow(int_type ch) override
    {
        auto tmp_ch = static_cast<char_type>(ch);
        return _sink.write(&tmp_ch, 1) == 1 ? ch : traits_type::eof();
    }

    int sync() override { _sink.flush(); return 0; }

    std::streamsize xsputn(const char_type* s, std::streamsize n) override { return _sink.write(s, n); }

private:
    Sink _sink;
};


/**
 * Implementation of output stream object.
 *
 * This class inherits from <code>std::basic_ostream</code> and can
 * be used as one.
 *
 * This object uses the <code>Sink</code> provided as a template parameter
 * to write data to. The <code>Sink</code> type should follow the
 * specifications of either nova::sink or nova::out_buffer_provider.
 *
 * @tparam Sink sink object to use to write data.
 * @tparam Buffering Buffer size to be used. It must be nova::non_buffered
 *                   if nova::out_buffer_provider is used as
 *                   <code>Sink</code>
 * @tparam Traits character traits type to be used in this stream.
 *
 * @see sink
 * @see out_buffer_provider
 */
template<typename Sink, typename Buffering = non_buffered, typename Traits = std::char_traits<typename Sink::char_type>>
class outstream : public std::basic_ostream<typename Sink::char_type, Traits>
{
    typedef basic_outbuf<Sink, Buffering, Traits>                _outbuf_type;
    typedef std::basic_ostream<typename Sink::char_type, Traits> _ostream_type;
public:
    /**
     * Character type to be used by this stream.
     */
    typedef typename Sink::char_type       char_type;
    /**
     * Type of character traits class to be used by this stream
     */
    typedef Traits 					       traits_type;
    /**
     * traits_type::int_type
     */
    typedef typename traits_type::int_type int_type;
    /**
     * traits_type::pos_type
     */
    typedef typename traits_type::pos_type pos_type;
    /**
     * traits_type::off_type
     */
    typedef typename traits_type::off_type off_type;

    /**
     * Main constructor.
     *
     * This generic constructor will pass received arguments to <code>Sink</code>
     * constructor.
     *
     * @tparam Args types of the arguments to forward to <code>Sink</code> constructor
     *
     * @param args Arguments to be forwarded to construct the <code>Sink</code>
     */
    template <class... Args>
    explicit outstream(Args&&... args) : _ostream_type{}
    {
        _ostream_type::rdbuf(new _outbuf_type{std::forward<Args>(args)...});
    }
    /**
     * Move constructor
     *
     * @param other <code>outstream</code> which contents and state will be acquired
     *              by created object.
     */
    outstream(outstream&& other) noexcept : _ostream_type{}
    {
        _ostream_type::rdbuf(other.rdbuf());
        other.rdbuf(nullptr);
    }
    /**
     * Move assignment operator
     *
     * @param other <code>outstream</code> which contents and state will be acquired
     *              by created object.
     * @return reference to self
     */
    outstream& operator=(outstream&& other) noexcept
    {
        _ostream_type::rdbuf(other.rdbuf());
        other.rdbuf(nullptr);
        return *this;
    }

    /**
     * Destructor.
     */
    ~outstream() noexcept override { delete _ostream_type::rdbuf(); }

    /**
     * Provides access to the reference to the <code>Sink</code> instance
     * associated with this stream.
     *
     * @return reference to the <code>Sink</code> instance.
     */
    Sink& operator*() { return buf()->operator*(); }
    /**
     * Provides access to the pointer to the <code>Sink</code> instance
     * associated with this stream.
     *
     * @return pointer to the <code>Sink</code> instance.
     */
    Sink* operator->() { return buf()->operator->(); }

    /**
     * Provides access to the constant reference to the <code>Sink</code>
     * instance associated with this stream.
     *
     * @return const reference to the <code>Sink</code> instance.
     */
    const Sink& operator*() const { return buf()->operator*(); }
    /**
     * Provides access to the constant pointer to the <code>Sink</code>
     * instance associated with this stream.
     *
     * @return const pointer to the <code>Sink</code> instance.
     */
    const Sink* operator->() const { return buf()->operator->(); }

private:
    inline _outbuf_type *buf() { return static_cast<_outbuf_type*>(_ostream_type::rdbuf()); }
    inline const _outbuf_type *buf() const { return static_cast<const _outbuf_type*>(_ostream_type::rdbuf()); }
};

template<typename Source, typename Buffering, typename Traits, typename Enable = void>
class basic_inbuf;

template<typename Source, typename Buffering, typename Traits, typename Enable>
class basic_inbuf : public std::basic_streambuf<typename Source::char_type, Traits>
{
    typedef std::basic_streambuf<typename Source::char_type, Traits> _buf_type;
public:
    typedef typename Source::char_type char_type;
    typedef Traits                     traits_type;
    typedef typename Traits::int_type  int_type;
    typedef typename Traits::pos_type  pos_type;
    typedef typename Traits::off_type  off_type;

    template <class... Args>
    explicit basic_inbuf(Args&&... args) :
            _source{std::forward<Args>(args)...}, _buffer{new char_type[Buffering::buf_size]} {}

    ~basic_inbuf() noexcept override { delete[] _buffer; }

    basic_inbuf(const basic_inbuf& ) = delete;
    basic_inbuf(basic_inbuf&& other) = delete;

    basic_inbuf& operator=(const basic_inbuf& ) = delete;
    basic_inbuf& operator=(basic_inbuf&& ) = delete;

    Source& operator*() { return _source; }
    Source* operator->() { return &_source; }

    const Source& operator*() const { return _source; }
    const Source* operator->() const { return &_source; }

    void reset() { }

protected:
    int_type underflow() override
    {
        std::streamsize new_size = _source.read(_buffer, Buffering::buf_size);
        if (new_size == 0) return traits_type::eof();
        _buf_type::setg(_buffer, _buffer, _buffer + new_size);
        return traits_type::to_int_type(*_buffer);
    }

    int_type pbackfail(int_type ch) override
    {
        if (_buf_type::egptr() <= _buf_type::eback()) return traits_type::eof();
        _buf_type::gbump(-1);
        return ch;
    }

private:
    Source _source;
    char_type *_buffer;
};

template<typename Source, typename Traits, typename Enable>
class basic_inbuf<Source, non_buffered, Traits, Enable> :
        public std::basic_streambuf<typename Source::char_type, Traits>
{
    typedef std::basic_streambuf<typename Source::char_type, Traits> _buf_type;
public:
    typedef typename Source::char_type char_type;
    typedef Traits                     traits_type;
    typedef typename Traits::int_type  int_type;
    typedef typename Traits::pos_type  pos_type;
    typedef typename Traits::off_type  off_type;

    template <class... Args>
    explicit basic_inbuf(Args&&... args) : _source{std::forward<Args>(args)...} {}

    ~basic_inbuf() noexcept override = default;

    basic_inbuf(const basic_inbuf& ) = delete;
    basic_inbuf(basic_inbuf&& other) = delete;

    basic_inbuf& operator=(const basic_inbuf& ) = delete;
    basic_inbuf& operator=(basic_inbuf&& ) = delete;

    Source& operator*() { return _source; }
    Source* operator->() { return &_source; }

    const Source& operator*() const { return _source; }
    const Source* operator->() const { return &_source; }

    void reset() { }

protected:
    int_type underflow() override
    {
        if (_source.read(&_ch, 1) == 0) return traits_type::eof();
        _buf_type::setg(&_ch, &_ch, &_ch + 1);
        return traits_type::to_int_type(_ch);
    }

    int_type pbackfail(int_type ch) override
    {
        if (_buf_type::egptr() <= _buf_type::eback()) return traits_type::eof();
        _buf_type::gbump(-1);
        return ch;
    }

private:
    Source _source;
    char_type _ch;
};

template<typename Source, typename Traits>
class basic_inbuf<Source, non_buffered, Traits,
                  typename std::enable_if_t<std::is_same<typename Source::category, in_buffer_provider>::value>> :
        public std::basic_streambuf<typename Source::char_type, Traits>
{
    typedef std::basic_streambuf<typename Source::char_type, Traits> _buf_type;
public:
    typedef typename Source::char_type char_type;
    typedef Traits                     traits_type;
    typedef typename Traits::int_type  int_type;
    typedef typename Traits::pos_type  pos_type;
    typedef typename Traits::off_type  off_type;

    template <class... Args>
    explicit basic_inbuf(Args&&... args) : _source{std::forward<Args>(args)...} {}

    ~basic_inbuf() noexcept override = default;

    basic_inbuf(const basic_inbuf& ) = delete;
    basic_inbuf(basic_inbuf&& other) = delete;

    basic_inbuf& operator=(const basic_inbuf& ) = delete;
    basic_inbuf& operator=(basic_inbuf&& ) = delete;

    Source& operator*() { return _source; }
    Source* operator->() { return &_source; }

    const Source& operator*() const { return _source; }
    const Source* operator->() const { return &_source; }

    void reset() { _buf_type::setg(_buf_type::eback(), _buf_type::eback(), _buf_type::egptr()); }

protected:
    int_type underflow() override
    {
#if __cplusplus > 201700L
        auto [buf, size] = _source.get_in_buffer();
        if (!buf || size <= 0) return traits_type::eof();
        auto non_const_buf = const_cast<char_type*>(buf);
        _buf_type::setg(non_const_buf, non_const_buf, non_const_buf + size);
        return traits_type::to_int_type(*_buf_type::eback());
#else
        auto res = _source.get_in_buffer();
        if (!res.first || res.second <= 0) return traits_type::eof();
        auto non_const_buf = const_cast<char_type*>(res.first);
        _buf_type::setg(non_const_buf, non_const_buf, non_const_buf + res.second);
        return traits_type::to_int_type(*_buf_type::eback());
#endif
    }

    int_type pbackfail(int_type ch) override
    {
        if (_buf_type::egptr() <= _buf_type::eback()) return traits_type::eof();
        _buf_type::gbump(-1);
        return ch;
    }

private:
    Source _source;
};

/**
 * Implementation of input stream object.
 *
 * This class inherits from <code>std::basic_istream</code> and can
 * be used as one.
 *
 * This object uses the <code>Source</code> provided as a template parameter
 * to write data to. The <code>Source</code> type should follow the
 * specifications of wither nova::source or nova::in_buffer_provider.
 *
 * @tparam Source source type to use to read data from.
 * @tparam Buffering Buffer size to be used. It must be nova::non_buffered
 *                   if <code>in_buffer_provider</code> is used as
 *                   <code>Source</code>
 * @tparam Traits character traits type to be used in this stream.
 *
 * @see source
 * @see in_buffer_provider
 */
template<typename Source, typename Buffering = non_buffered,
         typename Traits = std::char_traits<typename Source::char_type>>
class instream : public std::basic_istream<typename Source::char_type, Traits>
{
    typedef basic_inbuf<Source, Buffering, Traits>                 _inbuf_type;
    typedef std::basic_istream<typename Source::char_type, Traits> _istream_type;
public:
    /**
     * Character type to be used by this stream.
     */
    typedef typename Source::char_type 	   char_type;
    /**
     * Type of character traits class to be used by this stream
     */
    typedef Traits 					       traits_type;
    /**
     * traits_type::int_type
     */
    typedef typename traits_type::int_type int_type;
    /**
     * traits_type::pos_type
     */
    typedef typename traits_type::pos_type pos_type;
    /**
     * traits_type::off_type
     */
    typedef typename traits_type::off_type off_type;

    /**
     * Main constructor.
     *
     * This generic constructor will pass received arguments to <code>Source</code>
     * constructor.

     * @tparam Args types of the arguments to forward to <code>Source</code> constructor
     *
     * @param args Arguments to be forwarded to construct the <code>Source</code>
     */
    template <typename... Args>
    explicit instream(Args&&... args) : _istream_type{}
    {
        _istream_type::rdbuf(new _inbuf_type{std::forward<Args>(args)...});
    }
    /**
     * Copying is prohibited.
     */
    instream(const instream& ) = delete;
    /**
     * Move constructor
     *
     * @param other <code>instream</code> which contents and state will be acquired
     *              by created object.
     */
    instream(instream&& other) noexcept : _istream_type{}
    {
        _istream_type::rdbuf(other.rdbuf());
        other.rdbuf(nullptr);
    }

    /**
     * Move assignment operator
     *
     * @param other <code>instream</code> which contents and state will be acquired
     *              by created object.
     * @return reference to self
     */
    instream& operator=(instream&& other) noexcept
    {
        _istream_type::rdbuf(other.rdbuf());
        other.rdbuf(nullptr);
        return *this;
    }

    /**
     * Destructor
     */
    ~instream() noexcept override { delete _istream_type::rdbuf(); }

    /**
     * Provides access to the reference to the <code>Source</code> instance
     * associated with this stream.
     * @return reference to the <code>Source</code> instance.
     */
    Source& operator*() { return buf()->operator*(); }
    /**
     * Provides access to the pointer to the <code>Source</code> instance
     * associated with this stream.
     * @return pointer to the <code>Source</code> instance.
     */
    Source* operator->() { return buf()->operator->(); }

    /**
     * Provides access to the constant reference to the <code>Source</code>
     * instance associated with this stream.
     * @return const reference to the <code>Source</code> instance.
     */
    const Source& operator*() const { return buf()->operator*(); }
    /**
     * Provides access to the constant pointer to the <code>Source</code>
     * instance associated with this stream.
     * @return const pointer to the <code>Source</code> instance.
     */
    const Source* operator->() const { return buf()->operator->(); }

private:
    inline _inbuf_type* buf() { return static_cast<_inbuf_type*>(_istream_type::rdbuf()); }
    inline const _inbuf_type* buf() const { return static_cast<const _inbuf_type*>(_istream_type::rdbuf()); }
};

template <typename Source, typename Category = void>
class device_source;

template <typename Source>
class device_source<Source, std::enable_if_t<std::is_same<typename Source::in_category, source>::value>>
{
public:
    typedef typename Source::char_type   char_type;
    typedef typename Source::in_category category;

    explicit device_source(Source& source) : _source{source} {}
    ~device_source() noexcept = default;

    auto read(const char_type* s, std::streamsize n) { return _source.read(s, n); }

private:
    Source& _source;
};
template <typename Source>
class device_source<Source, std::enable_if_t<std::is_same<typename Source::in_category, in_buffer_provider>::value>>
{
public:
    typedef typename Source::char_type   char_type;
    typedef typename Source::in_category category;

    explicit device_source(Source& source) : _source{source} {}
    ~device_source() noexcept = default;

    auto get_in_buffer() { return _source.get_in_buffer(); };

private:
    Source& _source;
};

template <typename Sink, typename Category = void>
class device_sink;

template <typename Sink>
class device_sink<Sink, std::enable_if_t<std::is_same<typename Sink::out_category, sink>::value>>
{
public:
    typedef typename Sink::char_type    char_type;
    typedef typename Sink::out_category category;

    explicit device_sink(Sink& sink) : _sink{sink} {}
    ~device_sink() noexcept = default;

    auto write(const char_type* s, std::streamsize n) { return _sink.write(s, n); }
    auto flush() { return _sink.flush();}

private:
    Sink& _sink;
};
template <typename Sink>
class device_sink<Sink, std::enable_if_t<std::is_same<typename Sink::out_category, out_buffer_provider>::value>>
{
public:
    typedef typename Sink::char_type    char_type;
    typedef typename Sink::out_category category;

    explicit device_sink(Sink& sink) : _sink{sink} {}
    ~device_sink() noexcept = default;

    auto get_out_buffer() { return _sink.get_out_buffer(); }
    auto flush(std::size_t size) { return _sink.flush(size);}

private:
    Sink& _sink;
};

/**
 * Type definition for output stream which can accept <code>device</code>.
 *
 * The purpose of <code>device_instream</code> and
 * <code>device_outstream</code> classes it to share the same instance of
 * <code>Device</code>.
 *
 * <code>Device</code> is following specifications of both <code>Sink</code>
 * as prescribed by nova::outstream and <code>Source</code> as prescribed by
 * nova::instream.
 *
 * <code>Device</code> class should have type definitions for
 * <code>in_category</code> (it must be either nova::source or
 * nova::in_buffer_provider) and <code>out_category</code> (it must be
 * either nova::sink or nova::out_buffer_provider)
 *
 * Class <code>device_outstream</code> can be created with instance of
 * <code>Device</code> class.
 *
 * @tparam Device source type to use to read data from.
 * @tparam Buffering Buffer size to be used. It must be nova::non_buffered
 *                   if <code>out_buffer_provider</code> is used as
 *                   <code>out_category</code>
 * @tparam Traits character traits type to be used in this stream.
 *
 * @see sink
 * @see source
 * @see out_buffer_provider
 * @see in_buffer_provider
 * @see device_instream
 */
template <typename Device, typename Buffering = non_buffered,
          typename Traits = std::char_traits<typename Device::char_type>>
using device_outstream = outstream<device_sink<Device>, Buffering, Traits>;

/**
 * Type definition for input stream which can accept <code>device</code>.
 *
 * The purpose of <code>device_instream</code> and
 * <code>device_outstream</code> classes it to share the same instance of
 * <code>Device</code>.
 *
 * <code>Device</code> is following specifications of both <code>Sink</code>
 * as prescribed by nova::outstream and <code>Source</code> as prescribed by
 * nova::instream.
 *
 * <code>Device</code> class should have type definitions for
 * <code>in_category</code> (it must be either nova::source or
 * nova::in_buffer_provider) and <code>out_category</code> (it must be
 * either nova::sink or nova::out_buffer_provider)
 *
 * Class <code>device_instream</code> can be created with instance of
 * <code>Device</code> class.
 *
 * @tparam Device source type to use to read data from.
 * @tparam Buffering Buffer size to be used. It must be nova::non_buffered
 *                   if <code>in_buffer_provider</code> is used as
 *                   <code>out_category</code>
 * @tparam Traits character traits type to be used in this stream.
 *
 * @see sink
 * @see source
 * @see out_buffer_provider
 * @see in_buffer_provider
 * @see device_outstream
 */
template <typename Device, typename Buffering = non_buffered,
          typename Traits = std::char_traits<typename Device::char_type>>
using device_instream = instream<device_source<Device>, Buffering, Traits>;

} // end of nova namespace

#endif // NOVA_IO_H
