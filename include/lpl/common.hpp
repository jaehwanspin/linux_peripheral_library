#ifndef __LPL_COMMON_HPP__
#define __LPL_COMMON_HPP__
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
#include <ostream>

namespace lpl
{


constexpr const char* __peri_path = "/sys/class";
constexpr const char* __gpio_path = "gpio";
constexpr const char* __i2c_path = "i2c";
constexpr const char* __pwm_path = "pwm";

template <typename _Ty, size_t _Size>
using arr_t = _Ty[_Size];

template <size_t _Size>
using ch8_t = arr_t<char, _Size>;

using file_descriptor_t = int;

/**
 * @author Jin
 * @brief bool value
 */
enum class value_t : uint8_t
{
    off = 0x00,
    on = 0x01
};

/**
 * @author Jin
 * @brief PWM polarity constants
 */
enum class polarity_t : uint8_t
{
    normal,
    inversed
};

}



std::ostream& operator<<(std::ostream& os, lpl::value_t val)
{
    os << static_cast<int>(val);
    return os;
}

#endif