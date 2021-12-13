#ifndef __LPL_GPIO_LED_HPP__
#define __LPL_GPIO_LED_HPP__

#include "../generic/led.hpp"
#include "../peripheral/soft_pwm.hpp"

namespace lpl
{
namespace gpio
{

using led = generic::led<soft_pwm, soft_pwm::kind>;
}    
}


#endif