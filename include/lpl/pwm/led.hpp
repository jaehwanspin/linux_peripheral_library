#ifndef __LPL_PWM_LED_HPP__
#define __LPL_PWM_LED_HPP__

#include "../generic/led.hpp"

namespace lpl
{
namespace pwm
{

using led = lpl::generic::led<pwm, pwm::kind>;

}
}
#endif