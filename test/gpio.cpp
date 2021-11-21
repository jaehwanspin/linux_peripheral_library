#include <iostream>
#include <chrono>

#include <lpl/peripheral/gpio.hpp>

int main(int argc, char **argv)
{
    lpl::gpio led(200, lpl::direction_t::out);
    lpl::gpio button(149, lpl::direction_t::in);
    button.set_edge(lpl::edge_t::falling);

    lpl::epoll::epoll epoll(button.value_fd(),
        lpl::epoll::events::et);

    auto now = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - now <= std::chrono::seconds(10))
    {
        epoll.wait(1);
        auto value = static_cast<bool>(button.value());
        led.write(value);
    }

    

    return 0;
}