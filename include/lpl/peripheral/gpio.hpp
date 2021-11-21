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

#include <thread>
#include <functional>
#include <fstream>

#include <etl/string.h>

#include <boost/filesystem.hpp>

#include "lpl/epoll/epoll.hpp"
#include "lpl/peripheral/common.hpp"

namespace lpl
{

/**
 * @author Jin
 * @brief GPIO device input/output direction
 */
enum class direction_t : uint32_t
{
    in,
    out
};


/**
 * @author Jin
 * @brief for gpio async event
 */
enum class edge_t : uint8_t
{
    none,
    rising,
    falling,
    both
};


/**
 * @author Jin
 * @brief General Input/Output device class
 */
class gpio
{
    friend class epoll::epoll;

public:
    explicit gpio(int idx);
    gpio(int idx, direction_t dir);
    gpio(gpio& other) = delete;
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
    file_descriptor_t value_fd() const;
    value_t read() const;
    ssize_t write(value_t value);
    ssize_t write(int value);
    ssize_t write(bool value);
    void set_direction(direction_t dir);
    void set_edge(edge_t ed);
    void set_active_low(bool val);

private:
    etl::string<255>  _device_fullpath;
    file_descriptor_t _value_fd;
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
    return boost::filesystem::exists(this->_device_fullpath.c_str());
}

/**
 * @author Jin
 * @brief setting full path ex) "/sys/class/gpio/gpio200"
 */
void gpio::_set_fullpath(int idx)
{
    boost::filesystem::path p(__peri_path);
    ch8_t<8> device_with_num = {
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
    boost::filesystem::path p(this->_device_fullpath.c_str());
    p /= "value";
    if (!boost::filesystem::exists(p))
        throw std::exception();
    do
    {
        this->_value_fd = ::open(p.string().c_str(), O_RDWR | O_NONBLOCK);
    } while (this->_value_fd == -1);
}

/**
 * @author Jin
 * @brief closing all file descriptors and destroy async input thread
 */
void gpio::_close_all()
{
    if (this->direction() == direction_t::out)
        this->write(false);
    ::close(this->_value_fd);
}

/**
 * @author Jin
 * @brief exports the GPIO device
 */
void gpio::_export_device()
{
    int idx = 0;
    boost::filesystem::path fullpath = lpl::__peri_path;
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
    boost::filesystem::path fullpath = lpl::__peri_path;
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
    ch8_t<2> val = { 0, };
    auto n = ::lseek64(this->_value_fd, 0, SEEK_SET);
    ::read(this->_value_fd, val, sizeof(val));
    return static_cast<value_t>(val[0] - 48);
}

direction_t gpio::direction() const
{
    direction_t result;

    ch8_t<4> val = { 0, };
    
    {
        boost::filesystem::path p(this->_device_fullpath.c_str());
        p /= "direction";
        std::ifstream ifs(p);
        ifs >> val;
    }

    if (0 == std::strcmp(val, "in"))
        result = direction_t::in;
    else if (0 == std::strcmp(val, "out"))
        result = direction_t::out;

    return result;
}

edge_t gpio::edge() const
{
    edge_t result;

    ch8_t<8> val = { 0, };
    
    {
        boost::filesystem::path p(this->_device_fullpath.c_str());
        p /= "edge";
        std::ifstream ifs(p);
        ifs >> val;
    }

    if (0 == std::strcmp(val, "none"))
        result = edge_t::none;
    else if (0 == std::strcmp(val, "rising"))
        result = edge_t::rising;
    else if (0 == std::strcmp(val, "falling"))
        result = edge_t::falling;
    else if (0 == std::strcmp(val, "both"))
        result = edge_t::both;

    return result;
}

bool gpio::active_low() const
{
    ch8_t<2> val = { 0, };

    {
        boost::filesystem::path p(this->_device_fullpath.c_str());
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

file_descriptor_t gpio::value_fd() const
{
    return this->_value_fd;
}

ssize_t gpio::write(value_t value)
{
    ch8_t<3> val = {
        static_cast<char>(static_cast<int>(value)  + static_cast<int>('0')),
        '\n',
        0
    };
    return ::write(this->_value_fd, val, std::strlen(val) + 1);
}

ssize_t gpio::write(int value)
{
    ch8_t<3> val = {
        static_cast<char>(value + '0'),
        '\n',
        0
    };
    return ::write(this->_value_fd, val, sizeof(val) + 1);
}

ssize_t gpio::write(bool value)
{
    ch8_t<3> val = {
        static_cast<char>(value + static_cast<int>('0')),
        '\n',
        0
    };
    return ::write(this->_value_fd, val, sizeof(val) + 1);
}

void gpio::set_direction(direction_t dir)
{
    boost::filesystem::path p(this->_device_fullpath.c_str());
    p /= "direction";
    std::ofstream ofs(p);

    switch (dir)
    {
    case decltype(dir)::in:
        ofs << "in" << std::endl;
        break;
    case decltype(dir)::out:
        ofs << "out" << std::endl;
        break;
    }
}

void gpio::set_edge(edge_t ed)
{
    if (this->direction() == direction_t::in)
    {
        boost::filesystem::path p(this->_device_fullpath.c_str());
        p /= "edge";
        std::ofstream ofs(p);

        switch (ed)
        {
        case decltype(ed)::none:
            ofs << "none" << std::endl;
            break;
        case decltype(ed)::rising:
            ofs << "rising" << std::endl;
            break;
        case decltype(ed)::falling:
            ofs << "falling" << std::endl;
            break;
        case decltype(ed)::both:
            ofs << "both" << std::endl;
            break;
        }

    }
}

void gpio::set_active_low(bool val)
{
    boost::filesystem::path p(this->_device_fullpath.c_str());
    p /= "edge";
    std::ofstream ofs(p);
    
    ofs << (val ? '1' : '0') << std::endl;
}


}


#endif