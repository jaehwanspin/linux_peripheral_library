#ifndef __LPL_PERIPHERAL_LED_HPP__
#define __LPL_PERIPHERAL_LED_HPP__

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
//                         _     _____ ____                          //
//                        | |   | ____|  _ \                         //
//                        | |   |  _| | | | |                        //
//                        | |___| |___| |_| |                        //
//                        |_____|_____|____/                         //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include <fstream>

#if __cplusplus >= 201703L
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif

#include <etl/string.h>
#include <etl/vector.h>

#include "../common.hpp"

namespace lpl
{
namespace led
{

/**
 * @author Jin
 * @brief built-in LED device class
 */
class led
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
    explicit led(const etl::string<64>& led_name);
    brightness_t brightness() const;
    brightness_t max_brightness() const;
    void set_brightness(brightness_t value);

private:
    void _set_fullpath(const etl::string<64>& led_name);
    void _open_all();
    void _close_all();

private:
    etl::string<256> _device_fullpath;
    
};

led::led(const etl::string<64>& led_name)
{
    this->_set_fullpath(led_name);
}


void led::_set_fullpath(const etl::string<64>& led_name)
{
    __fs_path p(__peri_path);
    p /= __led_path;
    p /= led_name.c_str();
    this->_device_fullpath = p.string().c_str();
}


brightness_t led::brightness() const
{
    brightness_t value = 0;
    __fs_path p(this->_device_fullpath.c_str());
    p /= "brightness";
    std::ifstream ifs(p);
    ifs >> value;
    return value;
}

brightness_t led::max_brightness() const
{
    brightness_t value = 0;
    __fs_path p(this->_device_fullpath.c_str());
    p /= "max_brightness";
    std::ifstream ifs(p);
    ifs >> value;
    return value;
}

void led::set_brightness(brightness_t value)
{
    __fs_path p(this->_device_fullpath.c_str());
    p /= "brightness";
    std::ofstream ofs(p);
    ofs << value;
}


}
}

#endif