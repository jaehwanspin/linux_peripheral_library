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

extern "C"
{
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <sys/termios.h>
}


#include <etl/string.h>

#include "../lpl_traits.hpp"
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
    constexpr parity_t none = 0x00;
    constexpr parity_t even = 0x01;
    constexpr parity_t odd = 0x02;
    constexpr parity_t mark = 0x03;
}

using stopbits_t = uint32_t;
namespace stopbits
{
    constexpr stopbits_t one = 0x00;
    constexpr stopbits_t two = 0x02;
}

using bytesize_t = uint32_t;
namespace bytesizes
{
    constexpr bytesize_t b5 = CS5;
    constexpr bytesize_t b6 = CS6;
    constexpr bytesize_t b7 = CS7;
    constexpr bytesize_t b8 = CS8;
}



class uart : lpl_traits::uart_traits
{
    
public:
    explicit uart(const etl::string<256>& device_name,
        baudrate_t b = baudrates::b9600, parity_t p = parities::none,
        stopbits_t s = stopbits::one, bytesize_t bs = bytesizes::b8,
        bool async = false);
    ~uart();

    file_descriptor_t fd() const;

    ssize_t read(void* buf, size_t size);
    ssize_t write(void* buf, size_t size);

private:
    void _set_fullpath(const etl::string<256>& device_name);
    void _open_all(baudrate_t b, parity_t p, stopbits_t s, bytesize_t bs, bool async);
    void _close_all();

private:  
    file_descriptor_t _fd;
    speed_t _speed;
    parity_t _parity;

    etl::string<256> _device_fullpath;
};


uart::uart(const etl::string<256>& device_name,
    baudrate_t b, parity_t p, stopbits_t s, bytesize_t bs, bool async)
{   
    this->_set_fullpath(device_name);
    this->_open_all(b, p, s, bs, async);
}

uart::~uart()
{
    this->_close_all();
}

void uart::_set_fullpath(const etl::string<256>& device_name)
{
    this->_device_fullpath = device_name;
}

void uart::_open_all(baudrate_t b, parity_t p, stopbits_t s, bytesize_t bs, bool async)
{
    int32_t open_flags = O_RDWR | O_NOCTTY;
    if (async) open_flags |= O_ASYNC | O_NONBLOCK;
    this->_fd = ::open(this->_device_fullpath.c_str(), open_flags);

    struct termios tty = { 0, };

    tty.c_cflag |= (CRTSCTS | CLOCAL | CREAD);
    tty.c_iflag |= (IGNPAR | ICRNL);
    tty.c_lflag |= ICANON;

    tty.c_cflag |= b;

    switch (s)
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
    
    switch (p)
    {
    case parities::none:
    {
        tty.c_cflag &= ~(PARENB | PARODD | CMSPAR);
        break;
    }
    case parities::even:
    {
        tty.c_cflag &= ~(PARODD | CMSPAR);
        tty.c_cflag |= PARENB;
        break;
    }
    case parities::odd:
    {
        tty.c_cflag &= ~CMSPAR;
        tty.c_cflag |= PARENB | PARODD;
        break;
    }
    case parities::mark:
    {
        tty.c_cflag |= PARENB | CMSPAR | PARODD;
        break;
    }
    }

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    tty.c_cflag |= bs;

    tty.c_cc[VINTR] = 0;    /* Ctrl-c */
    tty.c_cc[VQUIT] = 0;    /* Ctrl-\ */
    tty.c_cc[VERASE] = 0;   /* del */
    tty.c_cc[VKILL] = 0;    /* @ */
    tty.c_cc[VEOF] = 4;     /* Ctrl-d */
    tty.c_cc[VTIME] = 0;    /* inter-character timer unused */
    tty.c_cc[VMIN] = 1;     /* blocking read until 1 character arrives */
    tty.c_cc[VSWTC] = 0;    /* '\0' */
    tty.c_cc[VSTART] = 0;   /* Ctrl-q */
    tty.c_cc[VSTOP] = 0;    /* Ctrl-s */
    tty.c_cc[VSUSP] = 0;    /* Ctrl-z */
    tty.c_cc[VEOL] = 0;     /* '\0' */
    tty.c_cc[VREPRINT] = 0; /* Ctrl-r */
    tty.c_cc[VDISCARD] = 0; /* Ctrl-u */
    tty.c_cc[VWERASE] = 0;  /* Ctrl-w */
    tty.c_cc[VLNEXT] = 0;   /* Ctrl-v */
    tty.c_cc[VEOL2] = 0;    /* '\0' */

    ::tcflush(this->_fd, TCIFLUSH);
    if (::tcsetattr(this->_fd, TCSANOW, &tty) < 0)
        throw std::runtime_error(__FUNCTION__);
}

void uart::_close_all()
{
    ::close(this->_fd);
}

file_descriptor_t uart::fd() const
{
    return this->_fd;
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