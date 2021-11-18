#ifndef __LPL_SOFT_PWM_HPP__
#define __LPL_SOFT_PWM_HPP__
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

#include <pthread.h>

#include <thread>
#include <atomic>

#include "./gpio.hpp"

namespace lpl
{

class soft_pwm
{
public:
    explicit soft_pwm(int gpio_num);
    ~soft_pwm();

private:
    void         _control_handler();

public:
    bool         enable();
    uint64_t     period();
    uint64_t     duty_cycle();
    polarity_t   polarity();
    void         set_enable(bool val);
    void         set_period(uint64_t val);
    void         set_duty_cycle(uint64_t val);
    void         set_polarity(polarity_t val);

private:
    bool         _enable;
    uint64_t     _period;
    uint64_t     _duty_cycle;
    polarity_t   _polarity;
    gpio         _gpio;
    std::thread  _control_thr;
};

soft_pwm::soft_pwm(int gpio_num) :
    _gpio(gpio_num)
{
    this->_gpio.set_direction(direction_t::out);
}

soft_pwm::~soft_pwm()
{
    if (this->enable())
        this->set_enable(false);
}

void soft_pwm::_control_handler()
{
    while (this->_enable)
    {
        this->_gpio.write(true);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        this->_gpio.write(false);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

bool soft_pwm::enable()
{
    return this->_enable;
}

uint64_t soft_pwm::period()
{
    return this->_period;
}

uint64_t soft_pwm::duty_cycle()
{
    return this->_duty_cycle;
}

polarity_t soft_pwm::polarity()
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
        struct sched_param sch_param = { 0, };
        sch_param.sched_priority = ::sched_get_priority_max(SCHED_RR);
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

void soft_pwm::set_polarity(polarity_t val)
{
    this->_polarity = val;
}



}

#endif