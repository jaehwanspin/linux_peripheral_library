#include <iostream>
#include <chrono>

#include <lpl/peripheral/gpio.hpp>
#include <lpl/async/epoll.hpp>

int main(int argc, char **argv)
{
    lpl::gpio::gpio led(200, lpl::gpio::directions::out);
    lpl::gpio::gpio button(149, lpl::gpio::directions::in);
    button.set_edge(lpl::gpio::edges::falling);

    lpl::async::epoll epoll(led, lpl::async::events::et);

    auto now = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - now <= std::chrono::seconds(10))
    {
        epoll.wait(1);
        auto value = button.value();
        led.write(value);
    }

    return 0;
}