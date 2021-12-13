#ifndef __LPL_PERIPHERAL_LPL_TRAITS_HPP__
#define __LPL_PERIPHERAL_LPL_TRAITS_HPP__

#include <cinttypes>
#include <type_traits>

namespace lpl
{

namespace lpl_traits
{

struct gpio_traits { };
struct pwm_traits { };
struct i2c_traits { };
struct spi_traits { };
struct uart_traits { };

template <typename _Device, bool _Value>
struct is_gpio { static constexpr bool value = _Value; };
template <typename _Device, bool _Value>
struct is_pwm { static constexpr bool value = _Value; };
template <typename _Device, bool _Value>
struct is_i2c { static constexpr bool value = _Value; };
template <typename _Device, bool _Value>
struct is_spi { static constexpr bool value = _Value; };
template <typename _Device, bool _Value>
struct is_uart { static constexpr bool value = _Value; };

template <typename _Device>
struct is_gpio<_Device, std::is_same<typename _Device::kind, gpio_traits>::value>
{

};

template <typename _Device>
struct is_pwm<_Device, std::is_same<typename _Device::kind, pwm_traits>::value>
{

};

template <typename _Device>
struct is_i2c<_Device, std::is_same<typename _Device::kind, i2c_traits>::value>
{

};

template <typename _Device>
struct is_spi<_Device, std::is_same<typename _Device::kind, spi_traits>::value>
{

};

template <typename _Device>
struct is_uart<_Device, std::is_same<typename _Device::kind, uart_traits>::value>
{

};

#if __cplusplus >= 201703L
template <typename _Device>
constexpr bool is_gpio_v = is_gpio<_Device>::value;
template <typename _Device>
constexpr bool is_pwm_v = is_pwm<_Device>::value;
template <typename _Device>
constexpr bool is_i2c_v = is_i2c<_Device>::value;
template <typename _Device>
constexpr bool is_spi_v = is_spi<_Device>::value;
template <typename _Device>
constexpr bool is_uart_v = is_uart<_Device>::value;
#endif

}

}

#endif