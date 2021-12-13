#include <lpl/gpio/led.hpp>

int main(int argc, char** argv)
{
    using namespace std::literals;

    lpl::gpio::led led(27, 1'000, 100, 1, 200ms);
    led.set_brightness(100);
    std::this_thread::sleep_for(1s);
    led.set_brightness(50);
    std::this_thread::sleep_for(1s);
    led.set_brightness(0);
    std::this_thread::sleep_for(1s);

    return 0;
}