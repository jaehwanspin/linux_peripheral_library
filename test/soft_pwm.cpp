#include <iostream>

#include <lpl/peripheral/soft_pwm.hpp>

int main(int argc, char** argv)
{
    using namespace std::literals;

    lpl::soft_pwm sp(168);
    sp.set_period(1'000'000);
    sp.set_duty_cycle(0);
    sp.set_enable(true);

    uint64_t duty = 0;

    for (int i = 0; i <= 1'000'000; i += 100'000)
    {
        sp.set_duty_cycle(i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    for (int i = 1'000'000; i >= 0; i -= 100'000)
    {
        sp.set_duty_cycle(i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    

    // while (true)
    // {
    //     for (int i = 0; i <= 1000; i++)
    //     {
    //         duty += i * 1'000;
    //         sp.set_duty_cycle(duty);
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //     }
    //     for (int i = 1000; i >= 0; i--)
    //     {
    //         duty -= i * 1'000;
    //         sp.set_duty_cycle(duty);
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //     }
    // }

    return 0;
}
