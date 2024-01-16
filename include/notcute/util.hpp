#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <initializer_list>
#include <sstream>
#include <fstream>
#include <limits>
#include <queue>
#include <regex>
#include <iostream>
#include <cmath>
#include <tuple>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <iomanip>
//#include <termios.h>
//#include <unistd.h>
#include <mutex>
#include <cassert>
#include <atomic>
#include <sys/stat.h>

#if defined(__GNUC__) && __GNUC__ >= 9
#include <string_view>

static constexpr float NM_TO_FT = 6076.12f;

template <typename T>
constexpr auto type_name() noexcept
{
    std::string_view name = "Error: unsupported compiler", prefix, suffix;

#ifdef __clang__
    name = __PRETTY_FUNCTION__;
    prefix = "auto type_name() [T = ";
    suffix = "]";
#elif defined(__GNUC__)
    name = __PRETTY_FUNCTION__;
    prefix = "constexpr auto type_name() [with T = ";
    suffix = "]";
#elif defined(_MSC_VER)
    name = __FUNCSIG__;
    prefix = "auto __cdecl type_name<";
    suffix = ">(void) noexcept";
#endif
    name.remove_prefix(prefix.size());
    name.remove_suffix(suffix.size());
    return name;
}
#endif

inline bool fequals(float x, float y)
{
    return std::fabs(x - y) < std::numeric_limits<float>::epsilon();
}

inline bool isNumber(const std::string& text)
{
    // makes sure there are only optional digits, then an optional ".", then more optional digits after the decimal ("+" means atleast 1 or more, "*" means 0 or more)
    std::regex num("-?[0-9]*\\.*[0-9]*");

    return (regex_match(text, num) && text != "."); // checks that it is a number, and that it is not just a decimal (".")
}

// Copes len characters from str into dest. Will add a null terminator char at the end.

inline void strcpy_safe(char* dest, const std::string str, std::size_t len)
{
    std::size_t numCopied = str.copy(dest, len, 0);
    dest[numCopied] = '\0';
}

struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const std::pair<T1, T2>& p) const
    { 
        auto hash1 = std::hash<T1>{}(p.first); 
        auto hash2 = std::hash<T2>{}(p.second); 
        return hash1 ^ hash2; 
    } 
};

template<size_t WIDTH, size_t HEIGHT>
void flip_vertically(char (&buffer)[HEIGHT][WIDTH])
{
    for (unsigned i = 0; i < HEIGHT/2; ++i)
    {
        std::swap(buffer[i], buffer[HEIGHT-1-i]);
    }
}

template<size_t WIDTH, size_t HEIGHT>
void flip_horizontally(char (&buffer)[HEIGHT][WIDTH])
{
    for (unsigned i = 0; i < HEIGHT; ++i)
    {
        std::reverse(buffer[i], buffer[i]+WIDTH);
    }
}

template<size_t WIDTH, size_t HEIGHT>
void invert(char (&buffer)[HEIGHT][WIDTH])
{
    flip_vertically(buffer);
    flip_horizontally(buffer);
}

namespace util
{

template<size_t N>
uint32_t compress(uint32_t bits[N])
{
    assert(N > 0 && N <= 32); 

    uint32_t masked = 0;
    for (uint32_t i = 0; i < N; ++i)
    {
        masked |= bits[i] << i;
    }
    
    return masked;
}

template<typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
// typename std::enable_if<std::is_integral<T>::value,T>::type
T compress(std::initializer_list<bool> bits)
{
    assert(bits.size() <= sizeof(T)*8); 

    T masked = 0;
    int shift = 0;
    for (const auto& i : bits)
    {
        masked |= i << shift++;
    }
    
    return masked;
}


template<typename ... Args>
std::string string_format(const std::string& format, Args ... args )
{
    int size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

template<typename T>
std::string to_string_with_precision(const T value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << value;
    return out.str();
}

inline std::vector<uint8_t> hexToBytes(const std::string& hex)
{
    std::vector<uint8_t> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2)
    {
        std::string byteString = hex.substr(i,2);
        char byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

inline std::vector<std::string> tokenize(const std::string& str, char delim)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delim))
    {
        tokens.push_back(token);
    }
    return tokens;
}

inline std::pair<int,int> unpackKeyValuePair(const std::string& str, char delim)
{
    auto tokens = util::tokenize(str, delim);
    if (tokens.size() != 2)
    {
        throw std::runtime_error("Exception thrown: could not unpack key-value pair, msg not formatted as %d=%d");
    }

    int key = std::atoi(tokens[0].c_str());
    int value = std::atoi(tokens[1].c_str());
    return {key, value};
}

inline float normalize(float min, float max, float value)
{
    return ((value - min) / (max - min));
}

template<typename T>
inline T normalize(const T oldMin, const T oldMax, const T newMin, const T newMax, const T value)
{
    return newMin + ((newMax - newMin) / (oldMax - oldMin)) * (value - oldMin);
}

inline std::string readSourceFile(const std::string filepath)
{
    std::ifstream fstream(filepath);
    if (fstream.fail())
    {
        std::cout << "[" __FILE__ << ":" << __LINE__ << "] "
                  << "[" << __FUNCTION__ << "]: "
                  << "error - failed to open file " << filepath << std::endl;
    }
    std::stringstream ss;

    std::string line;
    while (getline(fstream, line)) { ss << line << std::endl; }

    return ss.str();
}

inline bool file_exists(const std::string& name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

template<typename T>
T clamp(const T& value, const T& min, const T& max)
{
    return std::max(min, std::min(value, max));
}

inline float smoothstep(float edge0, float edge1, float x)
{
    // Scale, bias and saturate x to 0..1 range
    x = util::clamp((x-edge0)/(edge1-edge0), 0.0f, 1.0f);
    return x*x*(3-2*x);
}

inline std::string toHex(const uint8_t* data, size_t len)
{
    std::stringstream ss;
    ss << "[";
    if (len > 0)
    {
        ss << std::hex << std::setfill('0') << std::setw(2) << 0u + data[0];
    }
    for (unsigned int i = 1; i < len; ++i)
    {
        ss << " " << std::hex << std::setfill('0') << std::setw(2) << 0u + data[i];
    }
    ss << "]";
    return ss.str();
}

inline std::string toHex(const std::string& data)
{
    return toHex(reinterpret_cast<const uint8_t*>(data.c_str()), data.size());
}

inline std::string toHex(const std::vector<uint8_t>& data)
{
    return toHex(data.data(), data.size());
}



// Command line argument parser
class InputParser
{
public:
    InputParser() = default;
    InputParser (int &argc, char **&argv)
    {
        for (int i=1; i < argc; ++i)
        {
            this->tokens.push_back(std::string(argv[i]));
        }
    }

    std::string getCmdOption(const std::string &option) const
    {
        auto itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end())
        {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }

    bool cmdOptionExists(const std::string &option) const
    {
        return std::find(this->tokens.begin(), this->tokens.end(), option)
               != this->tokens.end();
    }
private:
    std::vector <std::string> tokens;
};

/*
 * Takes a std::string and returns the lowercase version.
 * @param string to make lowercase
 */
inline std::string tolower(std::string string)
{
    std::transform(string.begin(), string.end(), string.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return string;
}

/*
 * Takes a std::string and returns the uppercase version.
 * @param string to make uppercase
 */
inline std::string toupper(std::string string)
{
    std::transform(string.begin(), string.end(), string.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return string;
}

template<typename T>
void removeFrom(T& container, const T& items)
{
    container.erase(
            std::remove_if(container.begin(), container.end(),
                [&](const auto& item){ return std::binary_search(std::begin(items), std::end(items), item); }
                ),
            container.end()
        );
}

// inline int openSerialPort(const char* path)
// {
//     int serial_port = open(path, O_RDWR);
//
//     if (serial_port == -1) { return serial_port; }
//
//     struct termios tty;
//
//     if (tcgetattr(serial_port, &tty) != 0)
//     {
//         LOG(ERROR) << util::string_format("Error %i from tcgetattr: %s", errno, strerror(errno));
//         return -1;
//     }
//
//     tty.c_cflag &= ~PARENB; // Disable parity
//     // tty.c_cflag |= PARENB;
//     tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication
//     tty.c_cflag &= ~CSIZE; // clear all size bits.
//     tty.c_cflag |= CS8; // 8 bits per byte
//     tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
//     tty.c_cflag |= CREAD | CLOCAL; //  Turn on read & ignore ctrl lines
//     tty.c_lflag &= ~ICANON; // Disable canonical mode
//     tty.c_lflag &= ~ECHO; // Disable echo
//     tty.c_lflag &= ~ECHOE; // Disable erasure
//     tty.c_lflag &= ~ECHONL; // Disable new-line echo
//     tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT, and SUSP
//     tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
//     tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL ); // Disable special handling of received bytes. we want raw data
//     tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. new line chars)
//     tty.c_oflag &= ~ONLCR; // Prevent conversions of new line to carriage return/line feed
//     tty.c_cc[VTIME] = 0; // both VTIME and VMIN = 0 means no blocking on read or write
//     tty.c_cc[VMIN] = 0;
//
//     // cfsetispeed(&tty, B57600); // baud rate
//     cfsetispeed(&tty, B9600); // baud rate
//
//     if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
//     {
//         LOG(ERROR) << util::string_format("Error %i from tcsetattr: %s", errno, strerror(errno));
//         return 0;
//     }
//     return serial_port;
// }

template<typename value_type, typename lock_type = std::mutex>
class update_guard
{
private:
    value_type& value;
    std::lock_guard<lock_type> lock;
public:
    explicit update_guard(value_type& val, lock_type& lk) : value(val), lock(lk) {}
    ~update_guard() {}

    value_type& operator*()  noexcept { return value; }
    value_type* operator->() noexcept { return &value; }

    update_guard(update_guard const&) = delete;
    update_guard& operator=(update_guard const&) = delete;

    update_guard& operator=(value_type const& rhs)
    {
        value = rhs;
        return *this;
    }
};

template<typename value_type, typename lock_type = std::mutex>
class synchronized_value
{
private:
    value_type value;
    lock_type lock;
public:
    template<typename ... Args>
    explicit synchronized_value(Args&& ... args) : value(std::forward<Args>(args)...) {}

    ~synchronized_value() {}

    synchronized_value(synchronized_value const&) = delete;
    synchronized_value& operator=(synchronized_value const&) = delete;
    synchronized_value& operator=(value_type const& v)
    {
        update_guard<value_type, lock_type> l(*this);
        value = v;
    }

    update_guard<value_type, lock_type> operator*()  { return update_guard<value_type, lock_type>(value, lock); }
    update_guard<value_type, lock_type> operator->() { return update_guard<value_type, lock_type>(value, lock); }
    
    value_type get() { update_guard<value_type, lock_type> l(value, lock); return value; } 

    template<typename F>
    auto apply(F&& func)
    {
        update_guard<value_type, lock_type> l(value, lock);
        return func(value);
    }
};


template<typename T, typename ErrorType = std::shared_ptr<std::runtime_error>>
struct Reader
{
	using reader_error_type = ErrorType;

    virtual std::pair<T,ErrorType> read() = 0;
};

template<typename T, typename ErrorType = std::shared_ptr<std::runtime_error>>
struct Writer
{
	using writer_error_type = ErrorType;

    virtual ErrorType write(const T&) = 0;
};

template<typename T>
struct ReaderWriter : public Reader<T>, public Writer<T> {};

template<typename T>
class Channel : public Reader<T>, public Writer<T>
{
public:
	using error = std::shared_ptr<std::runtime_error>;

    bool empty() { return isEmpty; }

    std::pair<T, error> read() override
    {
        if (isEmpty) { return {T{}, ERROR_CHANNEL_EMPTY}; }

		auto lock = *channel;
		auto& queue = *lock;
		T value = queue.front();
		queue.pop();
        isEmpty = queue.empty();
		return {value, nullptr};
    }

    error write(const T& value) override
	{
		auto lock = *channel;
		auto& queue = *lock;
		queue.push(value);
        isEmpty = false;
		return nullptr;
	}

private:
    util::synchronized_value<std::queue<T>> channel;
    std::atomic<bool> isEmpty = true;

	error ERROR_CHANNEL_EMPTY = std::make_shared<std::runtime_error>("channel is empty");
};

template<typename T>
using NonBlockingChannel = Channel<T>;

}// end namespace util
