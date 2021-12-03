#include <iostream>
#include <thread>

#include <lpl/peripheral/pwm.hpp>

int main(int argc, char** argv)
{
    using namespace std::literals;

    lpl::pwm::pwm p(0, 2);

    p.set_period(1'000'000);
    p.set_duty_cycle(0);
    p.set_enable(true);

    uint64_t duty = 0;

    for (int i = 0; i < 5; i++)
    {
        for (int i = 0; i <= 1'000'000; i += 10'000)
        {
            p.set_duty_cycle(i);
            std::this_thread::sleep_for(10ms);
        }

        for (int i = 1'000'000; i >= 0; i -= 10'000)
        {
            p.set_duty_cycle(i);
            std::this_thread::sleep_for(10ms);
        }
    }

    return 0;
}
