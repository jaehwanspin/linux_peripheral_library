#ifndef __LPL_PERIPHERAL_I2C_HPP__
#define __LPL_PERIPHERAL_I2C_HPP__

#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <cstring>
#include <cstdlib>
#if __cplusplus >= 201703L
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif

#include <etl/vector.h>
#include <etl/string.h>

#include "../common.hpp"

namespace lpl
{

namespace i2c
{

constexpr size_t i2c_max_devs = 127;

class i2c
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
    explicit i2c(int bus, int slave_addr);
    virtual ~i2c();

private:
    void _set_fullpath(int idx);
    void _open_all();
    void _close_all();

public:

    file_descriptor_t fd() const;
    uint8_t           slave_addr() const;
    static etl::vector<uint8_t, i2c_max_devs> scan();

    int write(uint8_t value);
    int write(uint8_t* value, size_t size);
    template <typename _Container, std::enable_if_t<
        std::is_same_v<typename _Container::value_type, uint8_t>>>
    int write(const _Container& value);
    
    int read(uint8_t* buf, size_t size);

    int write_reg(uint8_t reg, uint8_t value);
    int write_reg(uint8_t reg, uint8_t* value, size_t size);
    
    template <typename _Container, std::enable_if_t<
        std::is_same_v<typename _Container::value_type, uint8_t>>>
    int write_reg(uint8_t reg, const _Container& value);
    
    int read_reg(uint8_t reg, uint8_t* buf, size_t size);

    int mask_reg(uint8_t reg, uint8_t mask);

private:
    file_descriptor_t _fd;
    etl::string<256>  _device_fullpath;
    uint8_t           _slave_addr;
};

etl::vector<uint8_t, i2c_max_devs> i2c::scan()
{
    etl::vector<uint8_t, i2c_max_devs> addrs;
    
    
    return addrs;
}

i2c::i2c(int bus, int slave_addr) :
    _slave_addr(slave_addr)
{
    std::vector<uint8_t> vec;
    this->_set_fullpath(bus);
    this->_open_all();
}

i2c::~i2c()
{
    this->_close_all();
}

void i2c::_open_all()
{
    arr_t<uint8_t, 2> buf = { 0, };
    int ret = -1;
    ret = ::open64(this->_device_fullpath.c_str(), O_RDWR);
    if (ret < 0) throw std::exception();
    this->_fd = ret;
    
    ret = ::ioctl(this->_fd, I2C_SLAVE, static_cast<int>(this->_slave_addr));
}

void i2c::_close_all()
{
    ::close(this->_fd);
}

void i2c::_set_fullpath(int idx)
{
    __fs_path p(__dev_path);
    str8_t<8> device_with_num = { 0, };
    
    std::sprintf(device_with_num, "%s-%d", __i2c_path, idx);
    p /= device_with_num;
    this->_device_fullpath = p.string().c_str();
}


uint8_t i2c::slave_addr() const
{
    return this->_slave_addr;
}

int i2c::write(uint8_t value)
{
    // struct i2c_smbus_ioctl_data args = { 0, };
    // args.read_write = I2C_SMBUS_WRITE;
    // args.command = value;
    // args.size = I2C_SMBUS_BYTE;
    int written = 0;

    struct i2c_msg i2c_msg = { 0, };
    struct i2c_rdwr_ioctl_data i2c_data = { 0, };

    i2c_msg.len = 1;
    i2c_msg.addr = this->_slave_addr;
    i2c_msg.buf = &value;
    i2c_msg.flags = 0;

    i2c_data.nmsgs = 1;
    i2c_data.msgs = &i2c_msg;

    written = ::ioctl(this->_fd, I2C_RDWR, &i2c_data);

    return written;
}

int i2c::write(uint8_t* buf, size_t size)
{
    int written = 0;

    struct i2c_msg i2c_msg = { 0, };
    struct i2c_rdwr_ioctl_data i2c_data = { 0, };

    i2c_msg.len = static_cast<decltype(i2c_msg.len)>(size);
    i2c_msg.buf = buf;
    i2c_msg.addr = this->_slave_addr;
    i2c_msg.flags = 0;

    i2c_data.nmsgs = 1;
    i2c_data.msgs = &i2c_msg;

    written = ::ioctl(this->_fd, I2C_RDWR, &i2c_data);

    return written;
}

template <typename _Container, std::enable_if_t<
    std::is_same_v<typename _Container::value_type, uint8_t>>>
int i2c::write(const _Container &value)
{
    return ::write(this->_fd, value.data(), value.size());
}

int i2c::read(uint8_t* buf, size_t size)
{
    int res = 0;

    struct i2c_msg i2c_msg = { 0, };
    struct i2c_rdwr_ioctl_data i2c_data = { 0, };

    i2c_msg.len = static_cast<decltype(i2c_msg.len)>(size);
    i2c_msg.buf = buf;
    i2c_msg.addr = this->_slave_addr;
    i2c_msg.flags = I2C_M_RD;

    i2c_data.nmsgs = 1;
    i2c_data.msgs = &i2c_msg;

    res = ::ioctl(this->_fd, I2C_RDWR, &i2c_data);

    return res;
}


int i2c::write_reg(uint8_t reg, uint8_t value)
{
    arr_t<uint8_t, 2> buf = { reg, value };
    return this->write(buf, 2);
}

int i2c::write_reg(uint8_t reg, uint8_t* value, size_t size)
{
    auto buf = std::make_unique<uint8_t[]>(size + 1);
    buf[0] = reg;
    std::memcpy(buf.get() + 1, value, size);
    return this->write(buf.get(), size + 1);
}

template <typename _Container, std::enable_if_t<
    std::is_same_v<typename _Container::value_type, uint8_t>>>
int i2c::write_reg(uint8_t reg, const _Container& value)
{
    auto buf = std::make_unique<uint8_t[]>(value.size() + 1);
    buf[0] = reg;
    std::memcpy(buf.get() + 1, value.data(), value.size());
    return this->write(buf.get(), value.size() + 1);
}
    
int i2c::read_reg(uint8_t reg, uint8_t* buf, size_t size)
{
    this->write(reg);
    return this->read(buf, size);
}

int i2c::mask_reg(uint8_t reg, uint8_t mask)
{
    uint8_t val = 0;
    this->read_reg(reg, &val, 1);
    val |= mask;
    return this->write_reg(reg, val);
}


}

}

#endif