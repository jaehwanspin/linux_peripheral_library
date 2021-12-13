#ifndef __LPL_PERIPHERAL_SPI_HPP__
#define __LPL_PERIPHERAL_SPI_HPP__

extern "C"
{
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#include <linux/spi/spidev.h>
}

#include <cstdlib>
#if __cplusplus >= 201703L
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif

#include <etl/string.h>

#include "../lpl_traits.hpp"
#include "../common.hpp"

namespace lpl
{
namespace spi
{

using mode_t = uint8_t;
namespace modes
{
    constexpr mode_t cpha = SPI_CPHA;
    constexpr mode_t cpol = SPI_CPOL;
    constexpr mode_t mode_0 = SPI_MODE_0;
    constexpr mode_t mode_1 = SPI_MODE_1;
    constexpr mode_t mode_2 = SPI_MODE_2;
    constexpr mode_t mode_3 = SPI_MODE_3;
    constexpr mode_t cs_high = SPI_CS_HIGH;
    constexpr mode_t lsb_first = SPI_LSB_FIRST;
    constexpr mode_t three_wire = SPI_3WIRE;
    constexpr mode_t loop = SPI_LOOP;
    constexpr mode_t no_cs = SPI_NO_CS;
    constexpr mode_t ready = SPI_READY;
}

class spi : lpl_traits::spi_traits
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
    explicit spi(int chip, int idx, uint64_t freq, mode_t mode = modes::mode_3);

    ssize_t write(uint8_t* data, size_t size);
    ssize_t read(uint8_t* data, size_t size);
    ssize_t write_half(uint8_t* data, size_t size);
    ssize_t read_half(uint8_t* data, size_t size);
    ssize_t write_full(uint8_t* out, size_t out_size, uint8_t* in, size_t in_size);
    ssize_t read_full(uint8_t* in, size_t in_size, uint8_t* out, size_t out_size);

private:
    void _set_fullpath(int chip, int idx);
    void _open_all();
    void _close_all();

private:
    file_descriptor_t _fd;
    etl::string<256>  _device_fullpath;
    uint64_t          _frequency;
    mode_t            _mode;

};


spi::spi(int chip, int idx, uint64_t freq, mode_t mode) :
       _frequency(freq), _mode(mode)
{
    this->_set_fullpath(chip, idx);
    this->_open_all();
}

void spi::_set_fullpath(int chip, int idx )
{
    __fs_path p(__dev_path);
    str8_t<10> dev_name = { 0, };
    std::sprintf(dev_name, "spidev%d.%d", chip, idx);
    p /= dev_name;
    this->_device_fullpath = p.string().c_str();
}

void spi::_open_all()
{
    this->_fd = ::open(this->_device_fullpath.c_str(), O_RDWR);
    int res = 0;
    res = ::ioctl(this->_fd, SPI_IOC_WR_MODE, &this->_mode);
    res = ::ioctl(this->_fd, SPI_IOC_RD_MODE, &this->_mode);
    uint8_t bits_per_word = 8;
    res = ::ioctl(this->_fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
    res = ::ioctl(this->_fd, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word);
    res = ::ioctl(this->_fd, SPI_IOC_WR_MAX_SPEED_HZ, &this->_frequency);
    res = ::ioctl(this->_fd, SPI_IOC_RD_MAX_SPEED_HZ, &this->_frequency);
}

void spi::_close_all()
{
    ::close(this->_fd);
}


ssize_t spi::write(uint8_t* data, size_t size)
{
    return ::write(this->_fd, data, size);
}

ssize_t spi::read(uint8_t* data, size_t size)
{
    return ::read(this->_fd, data, size);
}

ssize_t spi::write_half(uint8_t* data, size_t size)
{
    return this->write(data, size);
}

ssize_t spi::read_half(uint8_t* data, size_t size)
{
    return this->read(data, size);
}

ssize_t spi::write_full(uint8_t* tx, size_t tx_size, uint8_t* rx, size_t rx_size)
{
    ssize_t res = -1;
    arr_t<struct spi_ioc_transfer, 2> transfers = { 0, };
    transfers[0].tx_buf = reinterpret_cast<uint64_t>(tx);
    transfers[0].len = tx_size;
    transfers[1].rx_buf = reinterpret_cast<uint64_t>(rx);
    transfers[1].len = rx_size;
    
    transfers[0].cs_change = 0;

    res = ::ioctl(this->_fd, SPI_IOC_MESSAGE(2), transfers);

    return res;
}

ssize_t spi::read_full(uint8_t* in, size_t in_size, uint8_t* out, size_t out_size)
{
    ssize_t res = 0;
    return res;
}



}
}

#endif