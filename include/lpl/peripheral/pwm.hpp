#ifndef __LPL_PERIPHERAL_PWM_HPP__
#define __LPL_PERIPHERAL_PWM_HPP__
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
//                       ______        ____  __                      //
//                      |  _ \ \      / /  \/  |                     //
//                      | |_) \ \ /\ / /| |\/| |                     //
//                      |  __/ \ V  V / | |  | |                     //
//                      |_|     \_/\_/  |_|  |_|                     //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <fcntl.h>

#include <cstring>
#include <cstdlib>
#include <fstream>
#if __cplusplus >= 201703L
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif

#include "../common.hpp"

#include <etl/string.h>

namespace lpl
{
namespace pwm
{

/**
 * @author Jin
 * @brief Pulse width modulation device class
 */
class pwm
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
    explicit pwm(int pwm_chip_num, int pwm_num);
    pwm(pwm& other) = delete;
    virtual ~pwm();

    bool         enable() const;
    uint64_t     period() const;
    uint64_t     duty_cycle() const;
    polarity_t   polarity() const;
    void         set_enable(bool val);
    void         set_period(uint64_t val);
    void         set_duty_cycle(uint64_t val);
    void         set_polarity(polarity_t val);

private:
    bool _exported();
    void _set_fullpath(int pwm_chip_num, int pwm_num);
    void _open_all();
    void _close_all();
    void _export_device();
    void _unexport_device();

private:
    etl::string<256>  _device_fullpath;

};

pwm::pwm(int pwm_chip_num, int pwm_num)
{
    this->_set_fullpath(pwm_chip_num, pwm_num);
    if (!this->_exported())
        this->_export_device();
    this->_open_all();
}

pwm::~pwm()
{
    this->_close_all();
    this->_unexport_device();
}

bool pwm::_exported()
{
    return __fs_exists(this->_device_fullpath.c_str());
}

void pwm::_set_fullpath(int pwm_chip_num, int pwm_num)
{
    __fs_path p(__peri_path);
    p /= __pwm_path;
    {
        str8_t<9> devicechip_with_num = { 0, };
        std::sprintf(devicechip_with_num, "%s%d", __pwmchip_path, pwm_chip_num);
        p /= devicechip_with_num;
    }
    {
        str8_t<9> device_with_num = { 0, };
        std::sprintf(device_with_num, "%s%d", __pwm_path, pwm_num);
        p /= device_with_num;
    }

    this->_device_fullpath = p.string().c_str();
}

void pwm::_open_all()
{
}

void pwm::_close_all()
{
}

void pwm::_export_device()
{
    int idx = 0;
    __fs_path fullpath(this->_device_fullpath.c_str());
    fullpath = fullpath.parent_path();
    fullpath /= "export";

    std::string device_name = this->_device_fullpath.c_str();
    auto temp_idx = device_name.find_last_of("pwm");
    auto idx_str = device_name.substr(temp_idx + 1);
    
    std::ofstream ofs(fullpath);
    ofs << idx_str << std::endl;

    while (!__fs_exists(this->_device_fullpath.c_str()));
}

void pwm::_unexport_device()
{
    int idx = 0;
    __fs_path fullpath(this->_device_fullpath.c_str());
    fullpath = fullpath.parent_path();
    fullpath /= "unexport";

    std::string device_name = this->_device_fullpath.c_str();
    auto temp_idx = device_name.find_last_of("pwm");
    auto idx_str = device_name.substr(temp_idx + 1);
    
    std::ofstream ofs(fullpath);
    ofs << idx_str << std::endl;
}


void pwm::set_enable(bool val)
{
    __fs_path p(this->_device_fullpath.c_str());
    p /= "enable";

    std::ofstream ofs(p);
    ofs << static_cast<int>(val) << std::endl;
}

void pwm::set_period(uint64_t val)
{
    __fs_path p(this->_device_fullpath.c_str());
    p /= "period";

    std::ofstream ofs(p);
    ofs << val << std::endl;
}

void pwm::set_duty_cycle(uint64_t val)
{
    __fs_path p(this->_device_fullpath.c_str());
    p /= "duty_cycle";

    std::ofstream ofs(p);
    ofs << val << std::endl;
}

void pwm::set_polarity(polarity_t val)
{
    __fs_path p(this->_device_fullpath.c_str());
    p /= "polarity";

    std::ofstream ofs(p);
    switch (val)
    {
    case polarities::normal:
        ofs << "normal" << std::endl;
        break;
    case polarities::inversed:
        ofs << "inversed" << std::endl;
        break;
    }
}

bool pwm::enable() const
{
    str8_t<2> val = { 0, };
    __fs_path p(this->_device_fullpath.c_str());
    p /= "enable";

    std::ifstream ifs(p);
    ifs >> val;
    return static_cast<bool>(val[0] - 48);
}

uint64_t pwm::period() const
{
    str8_t<21> val = { 0, };
    __fs_path p(this->_device_fullpath.c_str());
    p /= "period";
    std::ifstream ifs(p);
    ifs >> val;

    return std::strtoull(val, nullptr, 10);
}

uint64_t pwm::duty_cycle() const
{
    str8_t<21> val = { 0, };
    __fs_path p(this->_device_fullpath.c_str());
    p /= "duty_cycle";
    std::ifstream ifs(p);
    ifs >> val;

    return std::strtoull(val, nullptr, 10);
}

polarity_t pwm::polarity() const
{
    polarity_t result;

    str8_t<9> val = { 0, };
    
    {
        __fs_path p(this->_device_fullpath.c_str());
        p /= "polarity";
        std::ifstream ifs(p);
        ifs >> val;
    }

    if (0 == std::strcmp(val, "normal"))
        result = polarities::normal;
    else if (0 == std::strcmp(val, "inversed"))
        result = polarities::inversed;

    return result;
}


}
}

#endif