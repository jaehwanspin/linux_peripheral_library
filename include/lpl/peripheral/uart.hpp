#ifndef __LPL_PERIPHERAL_UART_HPP__
#define __LPL_PERIPHERAL_UART_HPP__

///////////////////////////////////////////////////////////////////////
//                       _   _   _    ____ _____                     //
//                      | | | | / \  |  _ \_   _|                    //
//                      | | | |/ _ \ | |_) || |                      //
//                      | |_| / ___ \|  _ < | |                      //
//                       \___/_/   \_\_| \_\|_|                      //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <termios.h>

#include <etl/string.h>

#include "../common.hpp"

namespace lpl
{
namespace uart
{

using baudrate_t = uint32_t;
namespace baudrates
{
    constexpr baudrate_t b300 = B300;
    constexpr baudrate_t b600 = B600;
    constexpr baudrate_t b1200 = B1200;
    constexpr baudrate_t b2400 = B2400;
    constexpr baudrate_t b4800 = B4800;
    constexpr baudrate_t b9600 = B9600;
    constexpr baudrate_t b19200 = B19200;
    constexpr baudrate_t b38400 = B38400;
    constexpr baudrate_t b57600 = B57600;
    constexpr baudrate_t b115200 = B115200;
    constexpr baudrate_t b460800 = B460800;
    constexpr baudrate_t b921600 = B921600;
}

using parity_t = uint32_t;
namespace parities
{
    constexpr parity_t none = 0x01;
    constexpr parity_t even = 0x02;
    constexpr parity_t odd = 0x03;
    constexpr parity_t mark = 0x04;
}

using stopbit_t = uint8_t;
namespace stopbits
{
    constexpr stopbit_t one = 0x01;
    constexpr stopbit_t two = 0x03;
}



class uart
{
    
public:
    explicit uart(const etl::string<256>& device_name,
        baudrate_t b = baudrates::b9600, parity_t p = parities::none,
        stopbit_t s = stopbits::one);
    ~uart();

    file_descriptor_t fd() const;

    ssize_t read(void* buf, size_t size);
    ssize_t write(void* buf, size_t size);
    void set_baudrate(baudrate_t baudrate);
    void set_parity(parity_t parity);
    void set_stopbit(stopbit_t stopbit);

private:
    void _set_fullpath(const etl::string<256>& device_name);
    void _open_all();
    void _close_all();

private:  
    file_descriptor_t _fd;
    speed_t _speed;
    parity_t _parity;

    etl::string<256> _device_fullpath;
};


uart::uart(const etl::string<256>& device_name,
    baudrate_t b, parity_t p, stopbit_t s)
{   
    this->_set_fullpath(device_name);
    this->_open_all();

    struct termios tty = { 0, };
    ::tcgetattr(this->_fd, &tty);
    std::memset(&tty, 0, sizeof(decltype(tty)));
    tty.c_cflag = baudrates::b115200 | CRTSCTS | CS8 | CLOCAL | CREAD;
    tty.c_iflag = IGNPAR | ICRNL;
    tty.c_oflag = 0;
    tty.c_lflag = ICANON;
    tty.c_cc[VINTR] = 0;     /* Ctrl-c */
    tty.c_cc[VQUIT] = 0;     /* Ctrl-\ */
    tty.c_cc[VERASE] = 0;     /* del */
    tty.c_cc[VKILL] = 0;     /* @ */
    tty.c_cc[VEOF] = 4;     /* Ctrl-d */
    tty.c_cc[VTIME] = 0;     /* inter-character timer unused */
    tty.c_cc[VMIN] = 1;     /* blocking read until 1 character arrives */
    tty.c_cc[VSWTC] = 0;     /* '\0' */
    tty.c_cc[VSTART] = 0;     /* Ctrl-q */
    tty.c_cc[VSTOP] = 0;     /* Ctrl-s */
    tty.c_cc[VSUSP] = 0;     /* Ctrl-z */
    tty.c_cc[VEOL] = 0;     /* '\0' */
    tty.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    tty.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    tty.c_cc[VWERASE] = 0;     /* Ctrl-w */
    tty.c_cc[VLNEXT] = 0;     /* Ctrl-v */
    tty.c_cc[VEOL2] = 0;     /* '\0' */

    ::tcflush(this->_fd, TCIFLUSH);
    ::tcsetattr(this->_fd, TCSANOW, &tty);

    // this->set_baudrate(b);
    // this->set_parity(s);
    // this->set_stopbit(s);
}

uart::~uart()
{
    this->_close_all();
}

void uart::_set_fullpath(const etl::string<256>& device_name)
{
    this->_device_fullpath = device_name;
}

void uart::_open_all()
{
    this->_fd = ::open(this->_device_fullpath.c_str(), O_RDWR | O_ASYNC);
}

void uart::_close_all()
{
    ::close(this->_fd);
}

file_descriptor_t uart::fd() const
{
    return this->_fd;
}


void uart::set_baudrate(baudrate_t baudrate)
{
    struct termios tty = { 0, };
    ::tcgetattr(this->_fd, &tty);

    tty.c_cflag |= (CLOCAL | CREAD | baudrate);

    if (::tcflush(this->_fd, TCIFLUSH) < 0)
        throw std::runtime_error("__FUNCTION__");
    if (::tcsetattr(this->_fd, TCSANOW, &tty) < 0)
        throw std::runtime_error("__FUNCTION__");
}

void uart::set_parity(parity_t parity)
{
    struct termios tty = { 0, };
    ::tcgetattr(this->_fd, &tty);

    switch (parity)
    {
    case parities::none:
    {
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        break;
    }
    case parities::even:
    {
        tty.c_cflag |= PARENB;
        tty.c_cflag &= ~PARODD;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS7;
        break;
    }
    case parities::odd:
    {
        tty.c_cflag |= PARENB;
        tty.c_cflag |= PARODD;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS7;
        break;
    }
    case parities::mark:
    {
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        break;
    }
    }

    if (::tcflush(this->_fd, TCIFLUSH) < 0)
        throw std::runtime_error("__FUNCTION__");
    if (::tcsetattr(this->_fd, TCSANOW, &tty) < 0)
        throw std::runtime_error(__FUNCTION__);
}

void uart::set_stopbit(stopbit_t stopbit)
{
    struct termios tty = { 0, };
    ::tcgetattr(this->_fd, &tty);

    switch (stopbit)
    {
    case stopbits::one:
    {
        tty.c_cflag &= ~CSTOPB;
        break;
    }
    case stopbits::two:
    {
        tty.c_cflag |= CSTOPB;
        break;
    }
    }

    if (::tcflush(this->_fd, TCIFLUSH) < 0)
        throw std::runtime_error("__FUNCTION__");
    if (::tcsetattr(this->_fd, TCSANOW, &tty) < 0)
        throw std::runtime_error("__FUNCTION__");
}


ssize_t uart::read(void* buf, size_t size)
{
    return ::read(this->_fd, buf, size);
}

ssize_t uart::write(void* buf, size_t size)
{
    return ::write(this->_fd, buf, size);
}



}
}



#endif