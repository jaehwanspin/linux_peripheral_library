#ifndef __LPL_PERIPHERAL_GPIO_HPP__
#define __LPL_PERIPHERAL_GPIO_HPP__
///////////////////////////////////////////////////////////////////////
//                    _     ___ _   _ _   ___  __                    //
//                   | |   |_ _| \ | | | | \ \/ /                    //
//                   | |    | ||  \| | | | |\  /                     //
//                   | |___ | || |\  | |_| |/  \                     //
//                   |_____|___|_| \_|\___//_/\_\                    //
//                                                                   //
//   ____  _____ ____  ___ ____  _   _ _____ ____      _    _        //
//  |  _ \| ____|  _ \|_ _|  _ \| | | | ____|  _ \    / \  | |       //
//  | |_) |  _| | |_) || || |_) | |_| |  _| | |_) |  / _ \ | |       //
//  |  __/| |___|  _ < | ||  __/|  _  | |___|  _ <  / ___ \| |___    //
//  |_|   |_____|_| \_\___|_|   |_| |_|_____|_| \_\/_/   \_\_____|   //
//                                                                   //
//             _     ___ ____  ____      _    ______   __            //
//            | |   |_ _| __ )|  _ \    / \  |  _ \ \ / /            //
//            | |    | ||  _ \| |_) |  / _ \ | |_) \ V /             //
//            | |___ | || |_) |  _ <  / ___ \|  _ < | |              //
//            |_____|___|____/|_| \_\/_/   \_\_| \_\|_|              //
//                                                                   //
//        _   _   _ _____ _   _  ___  ____            _ ___ _   _    //
//       / \ | | | |_   _| | | |/ _ \|  _ \   _      | |_ _| \ | |   //
//      / _ \| | | | | | | |_| | | | | |_) | (_)  _  | || ||  \| |   //
//     / ___ \ |_| | | | |  _  | |_| |  _ <   _  | |_| || || |\  |   //
//    /_/   \_\___/  |_| |_| |_|\___/|_| \_\ (_)  \___/|___|_| \_|   //
//                                                                   //
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//                          ____ ____ ___ ___                        //
//                         / ___|  _ \_ _/ _ \                       //
//                        | |  _| |_) | | | | |                      //
//                        | |_| |  __/| | |_| |                      //
//                         \____|_|  |___\___/                       //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <fcntl.h>

#include <fstream>
#include <cstring>
#include <cstdlib>

#if __cplusplus >= 201703L
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif

#include <etl/string.h>

#include "../common.hpp"

namespace lpl
{
namespace gpio
{

/**
 * @author Jin
 * @brief GPIO device input/output direction
 */
using direction_t = uint8_t;
namespace directions
{
    constexpr direction_t in = 0x00;
    constexpr direction_t out = 0x01;
}


/**
 * @author Jin
 * @brief for gpio async event
 */
using edge_t = uint8_t;
namespace edges
{
    constexpr edge_t none = 0x00;
    constexpr edge_t rising = 0x01;
    constexpr edge_t falling = 0x02;
    constexpr edge_t both = 0x03;
}


/**
 * @author Jin
 * @brief General Input/Output device class
 */
class gpio
{
    
#if __cplusplus >= 201703L
    using __fs_path = std::filesystem::path;
    template <typename _Ty>
    constexpr bool __fs_exists(_Ty ___path) { return std::filesystem::exists(___path); }
#else
    using __fs_path = boost::filesystem::path;
    template <typename _Ty>
    constexpr bool __fs_exists(_Ty ___path) { return boost::filesystem::exists(___path); }
#endif
    

public:
    explicit gpio(int idx);
    gpio(int idx, direction_t dir);
    gpio(gpio& other) = delete;

    gpio& operator<<(int val);
    gpio& operator<<(value_t val);
    gpio& operator<<(bool val);
    gpio& operator>>(value_t& val);

    virtual ~gpio();

private:
    bool _exported();
    void _set_fullpath(int idx);
    void _open_all();
    void _close_all();
    void _export_device();
    void _unexport_device();

public:
    direction_t direction() const;
    value_t value() const;
    edge_t edge() const;
    bool active_low() const;
    file_descriptor_t fd() const;
    value_t read() const;
    ssize_t write(value_t value);
    ssize_t write(int value);
    ssize_t write(bool value);
    void set_direction(direction_t dir);
    void set_edge(edge_t ed);
    void set_active_low(bool val);

private:
    etl::string<256>  _device_fullpath;
    file_descriptor_t _fd;
};


gpio::gpio(int idx)
{
    this->_set_fullpath(idx);
    if (!this->_exported())
        this->_export_device();
    this->_open_all();
}

gpio::gpio(int idx, direction_t dir)
{
    this->_set_fullpath(idx);
    if (!this->_exported())
        this->_export_device();
    this->_open_all();
    this->set_direction(dir);
}

gpio::~gpio()
{
    this->_close_all();
    this->_unexport_device();
}

/**
 * @author Jin
 * @brief check if the device already exported
 */
bool gpio::_exported()
{
    return __fs_exists(this->_device_fullpath.c_str());
}

/**
 * @author Jin
 * @brief setting full path ex) "/sys/class/gpio/gpio200"
 */
void gpio::_set_fullpath(int idx)
{
    __fs_path p(__peri_path);
    str8_t<8> device_with_num = {
        0,
    };
    
    std::sprintf(device_with_num, "%s%d", __gpio_path, idx);
    p /= __gpio_path;
    p /= device_with_num;
    this->_device_fullpath = p.string().c_str();
}

/**
 * @author Jin
 * @brief opening all file descriptors
 */
void gpio::_open_all()
{
    __fs_path p(this->_device_fullpath.c_str());
    p /= "value";
    if (!__fs_exists(p))
        throw std::exception();
    do
    {
        this->_fd = ::open(p.string().c_str(), O_RDWR | O_NONBLOCK);
    } while (this->_fd == -1);
}

/**
 * @author Jin
 * @brief closing all file descriptors and destroy async input thread
 */
void gpio::_close_all()
{
    if (this->direction() == directions::out)
        this->write(false);
    ::close(this->_fd);
}

/**
 * @author Jin
 * @brief exports the GPIO device
 */
void gpio::_export_device()
{
    int idx = 0;
    __fs_path fullpath = lpl::__peri_path;
    fullpath /= lpl::__gpio_path;
    fullpath /= "export";

    std::string device_name = this->_device_fullpath.c_str();
    auto temp_idx = device_name.find_last_of("gpio");
    auto idx_str = device_name.substr(temp_idx + 1);
    
    std::ofstream ofs(fullpath);
    ofs << idx_str << std::endl;
}

/**
 * @author Jin
 * @brief unexports the GPIO device
 */
void gpio::_unexport_device()
{
    int idx = 0;
    __fs_path fullpath = lpl::__peri_path;
    fullpath /= lpl::__gpio_path;
    fullpath /= "unexport";

    std::string device_name = this->_device_fullpath.c_str();
    auto temp_idx = device_name.find_last_of("gpio");
    auto idx_str = device_name.substr(temp_idx + 1);

    std::ofstream ofs(fullpath);
    ofs << idx_str << std::endl;
}


value_t gpio::read() const
{
    str8_t<2> val = { 0, };
    auto n = ::lseek64(this->_fd, 0, SEEK_SET);
    ::read(this->_fd, val, sizeof(val));
    return static_cast<value_t>(val[0] - 48);
}

direction_t gpio::direction() const
{
    direction_t result;

    str8_t<4> val = { 0, };
    
    {
        __fs_path p(this->_device_fullpath.c_str());
        p /= "direction";
        std::ifstream ifs(p);
        ifs >> val;
    }

    if (0 == std::strcmp(val, "in"))
        result = directions::in;
    else if (0 == std::strcmp(val, "out"))
        result = directions::out;

    return result;
}

edge_t gpio::edge() const
{
    edge_t result;

    str8_t<8> val = { 0, };
    
    {
        __fs_path p(this->_device_fullpath.c_str());
        p /= "edge";
        std::ifstream ifs(p);
        ifs >> val;
    }

    if (0 == std::strcmp(val, "none"))
        result = edges::none;
    else if (0 == std::strcmp(val, "rising"))
        result = edges::rising;
    else if (0 == std::strcmp(val, "falling"))
        result = edges::falling;
    else if (0 == std::strcmp(val, "both"))
        result = edges::both;

    return result;
}

bool gpio::active_low() const
{
    str8_t<2> val = { 0, };

    {
        __fs_path p(this->_device_fullpath.c_str());
        p /= "active_low";
        std::ifstream ifs(p);
        ifs >> val;
    }

    return static_cast<bool>(static_cast<int>(val[0]) - '0');
}

value_t gpio::value() const
{
    return this->read();
}

file_descriptor_t gpio::fd() const
{
    return this->_fd;
}

ssize_t gpio::write(value_t value)
{
    str8_t<3> val = {
        static_cast<char>(static_cast<int>(value)  + static_cast<int>('0')),
        '\n',
        0
    };
    return ::write(this->_fd, val, std::strlen(val) + 1);
}

ssize_t gpio::write(int value)
{
    str8_t<3> val = {
        static_cast<char>(value + '0'),
        '\n',
        0
    };
    return ::write(this->_fd, val, sizeof(val) + 1);
}

ssize_t gpio::write(bool value)
{
    str8_t<3> val = {
        static_cast<char>(value + static_cast<int>('0')),
        '\n',
        0
    };
    return ::write(this->_fd, val, sizeof(val) + 1);
}

void gpio::set_direction(direction_t dir)
{
    __fs_path p(this->_device_fullpath.c_str());
    p /= "direction";
    std::ofstream ofs(p);

    switch (dir)
    {
    case directions::in:
        ofs << "in" << std::endl;
        break;
    case directions::out:
        ofs << "out" << std::endl;
        break;
    }
}

void gpio::set_edge(edge_t ed)
{
    if (this->direction() == directions::in)
    {
        __fs_path p(this->_device_fullpath.c_str());
        p /= "edge";
        std::ofstream ofs(p);

        switch (ed)
        {
        case edges::none:
            ofs << "none" << std::endl;
            break;
        case edges::rising:
            ofs << "rising" << std::endl;
            break;
        case edges::falling:
            ofs << "falling" << std::endl;
            break;
        case edges::both:
            ofs << "both" << std::endl;
            break;
        }

    }
}

void gpio::set_active_low(bool val)
{
    __fs_path p(this->_device_fullpath.c_str());
    p /= "edge";
    std::ofstream ofs(p);
    
    ofs << (val ? '1' : '0') << std::endl;
}


gpio& gpio::operator<<(int val)
{
    this->write(static_cast<uint8_t>(val));
    return *this;
}

gpio& gpio::operator<<(value_t val)
{
    this->write(val);
    return *this;
}

gpio& gpio::operator<<(bool val)
{
    this->write(val);
    return *this;
}

gpio& gpio::operator>>(value_t& val)
{
    val = this->read();
    return *this;
}


}
}


#endif