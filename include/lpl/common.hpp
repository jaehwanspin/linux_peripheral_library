#ifndef __LPL_PERIPHERAL_COMMON_HPP__
#define __LPL_PERIPHERAL_COMMON_HPP__
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
//                 ____ ___  __  __ __  __  ___  _   _               //
//                / ___/ _ \|  \/  |  \/  |/ _ \| \ | |              //
//               | |  | | | | |\/| | |\/| | | | |  \| |              //
//               | |__| |_| | |  | | |  | | |_| | |\  |              //
//                \____\___/|_|  |_|_|  |_|\___/|_| \_|              //
//                                                                   //
///////////////////////////////////////////////////////////////////////


#include <fcntl.h>

#include <cinttypes>
#include <exception>

namespace lpl
{

constexpr const char* __dev_path = "/dev";
constexpr const char* __peri_path = "/sys/class";
constexpr const char* __gpio_path = "gpio";
constexpr const char* __i2c_path = "i2c";
constexpr const char* __pwmchip_path = "pwmchip";
constexpr const char* __pwm_path = "pwm";
constexpr const char* __led_path = "leds";

template <typename _Ty, size_t _Size>
using arr_t = _Ty[_Size];

using ch8_t = char;

template <size_t _Size>
using str8_t = arr_t<ch8_t, _Size>;

using file_descriptor_t = int;

template <typename _Ty, size_t _Size>
struct buf_t
{
    arr_t<_Ty, _Size> buf;
    size_t            size;
};

template <size_t _Size>
struct bytebuf_t : buf_t<uint8_t, _Size> { };


namespace gpio
{
/**
 * @author Jin
 * @brief bool value
 */
using value_t = uint8_t;
namespace values
{
    constexpr value_t low = 0x00;
    constexpr value_t high = 0x01;
}
}

namespace pwm
{
/**
 * @author Jin
 * @brief PWM polarity constants
 */
using polarity_t = uint8_t;
namespace polarities
{
    constexpr polarity_t normal = 0x00;
    constexpr polarity_t inversed = 0x01;
}

}


namespace led
{
using brightness_t = uint32_t;
}

}

#endif