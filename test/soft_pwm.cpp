#include <iostream>

#include <lpl/peripheral/pwm.hpp>
#include <lpl/peripheral/soft_pwm.hpp>

int main(int argc, char** argv)
{
    using namespace std::literals;

    lpl::gpio::soft_pwm led1(17);
    lpl::gpio::soft_pwm led2(18);
    lpl::gpio::soft_pwm led3(27);

    led1.set_period(1'000);
    led1.set_duty_cycle(0);
    led1.set_enable(true);
    led2.set_period(1'000);
    led2.set_duty_cycle(0);
    led2.set_enable(true);
    led3.set_period(1'000);
    led3.set_duty_cycle(0);
    led3.set_enable(true);

    for (int i = 0; i < 5; i++)
    {
        for (int i = 0; i <= 1'000; i += 1)
        {
            led1.set_duty_cycle(i);
            led2.set_duty_cycle(i);
            led3.set_duty_cycle(i);
            lpl::realtime::delay_for(1ms);
        }
    
        for (int i = 1'000; i >= 0; i -= 1)
        {
            led1.set_duty_cycle(i);
            led2.set_duty_cycle(i);
            led3.set_duty_cycle(i);
            lpl::realtime::delay_for(1ms);
        }
    }

    return 0;
}
