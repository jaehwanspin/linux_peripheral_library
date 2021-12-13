#ifndef __LPL_PERIPHERAL_SOFT_PWM_HPP__
#define __LPL_PERIPHERAL_SOFT_PWM_HPP__
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
//          ____   ___  _____ _____   ______        ____  __         //
//         / ___| / _ \|  ___|_   _| |  _ \ \      / /  \/  |        //
//         \___ \| | | | |_    | |   | |_) \ \ /\ / /| |\/| |        //
//          ___) | |_| |  _|   | |   |  __/ \ V  V / | |  | |        //
//         |____/ \___/|_|     |_|   |_|     \_/\_/  |_|  |_|        //
//                                                                   //
///////////////////////////////////////////////////////////////////////
#include <thread>
#include <atomic>

#include "../lpl_traits.hpp"
#include "../util.hpp"
#include "../common.hpp"
#include "./gpio.hpp"

namespace lpl
{
namespace gpio
{

struct soft_pwm
{
    using kind = lpl_traits::gpio_traits;

public:
    explicit soft_pwm(int gpio_num);
    soft_pwm(int gpio_num, uint64_t period, uint64_t duty_cycle);
    ~soft_pwm();

private:
    void         _control_handler();

public:
    bool         enable() const;
    uint64_t     period() const;
    uint64_t     duty_cycle() const;
    pwm::polarity_t   polarity() const;
    void         set_enable(bool val);
    void         set_period(uint64_t val);
    void         set_duty_cycle(uint64_t val);
    void         set_polarity(pwm::polarity_t val);

private:
    std::atomic_bool        _enable;
    std::atomic_uint64_t    _period;
    std::atomic_uint64_t    _duty_cycle;
    std::atomic<pwm::polarity_t> _polarity;
    gpio                    _gpio;
    std::thread             _control_thr;
};

soft_pwm::soft_pwm(int gpio_num) :
    _gpio(gpio_num),
    _polarity(pwm::polarities::normal)
{
    this->_gpio.set_direction(directions::out);
}

soft_pwm::soft_pwm(int gpio_num, uint64_t period, uint64_t duty_cycle) :
    _gpio(gpio_num),
    _period(period),
    _duty_cycle(duty_cycle),
    _polarity(pwm::polarities::normal)
{
    this->_gpio.set_direction(directions::out);
}

soft_pwm::~soft_pwm()
{
    if (this->enable())
        this->set_enable(false);
}

void soft_pwm::_control_handler()
{
    using ns = std::chrono::nanoseconds;
    auto now = std::chrono::high_resolution_clock::now();
    while (this->_enable)
    {
        auto peri_nanosec = std::nano::den / this->_period;
        auto percent = (this->_duty_cycle * 100) / this->_period;
        auto duty_nanosec = peri_nanosec * percent / 100;

        switch (this->_polarity)
        {
        case pwm::polarities::normal:
        {
            this->_gpio.write(true);
            now += ns(peri_nanosec - (peri_nanosec - duty_nanosec));
            std::this_thread::sleep_until(now);
            this->_gpio.write(false);
            now += ns(peri_nanosec - duty_nanosec);
            std::this_thread::sleep_until(now);
            break;
        }
        case pwm::polarities::inversed:
        {
            this->_gpio.write(true);
            now += ns(peri_nanosec - duty_nanosec);
            std::this_thread::sleep_until(now);
            this->_gpio.write(false);
            now += ns(peri_nanosec - (peri_nanosec - duty_nanosec));
            std::this_thread::sleep_until(now);
            break;
        }
        }
    }
}

bool soft_pwm::enable() const
{
    return this->_enable;
}

uint64_t soft_pwm::period() const
{
    return this->_period;
}

uint64_t soft_pwm::duty_cycle() const
{
    return this->_duty_cycle;
}

pwm::polarity_t soft_pwm::polarity() const
{
    return this->_polarity;
}

void soft_pwm::set_enable(bool val)
{
    this->_enable = val;
    if (this->_enable)
    {
        this->_control_thr =
            std::thread(&soft_pwm::_control_handler, this);
        auto pthr_fd = this->_control_thr.native_handle();
        struct sched_param sch_param = { ::sched_get_priority_max(SCHED_RR) };
        ::pthread_setschedparam(pthr_fd, SCHED_RR, &sch_param);
    }
    else
    {
        if (this->_control_thr.joinable())
            this->_control_thr.join();
    }
}

void soft_pwm::set_period(uint64_t val)
{
    this->_period = val;
}

void soft_pwm::set_duty_cycle(uint64_t val)
{
    this->_duty_cycle = val;
}

void soft_pwm::set_polarity(pwm::polarity_t val)
{
    this->_polarity = val;
}



}
}

#endif