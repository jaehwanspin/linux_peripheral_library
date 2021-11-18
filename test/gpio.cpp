#include <iostream>
#include <memory>
#include <thread>
#include <array>

#include <boost/asio.hpp>

#include <linux/gpio.h>

#include <lpl/gpio.hpp>
#include <lpl/soft_pwm.hpp>
#include <lpl/epoll/epoll.hpp>

int main(int argc, char **argv)
{
    // lpl::gpio gpio(200);
    // gpio.set_direction(lpl::direction_t::out);

    // bool value = false;

    // for (int i = 0; i < 200; i++)
    // {
    //     int ret = gpio.write(value);
    //     value = !value;
    //     std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // }

    // boost::asio::io_context io_ctx;
    // boost::asio::serial_port sp(io_ctx);

    // sp.open(argv[1]);

    // auto is_open = sp.is_open();
    // if (!is_open) std::cerr << "fuck " << argv[1] << std::endl; 

    // boost::asio::serial_port::baud_rate br(115200);
    // sp.set_option(br);

    // for (int i = 0; i < 10; i++)
    // {
    //     std::string fucking = "oioi!!!!\r\n";
    //     boost::asio::write(sp, boost::asio::buffer(fucking));
    //     std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // }

    lpl::gpio gpio(149);
    gpio.set_direction(lpl::direction_t::in);
    gpio.set_edge(lpl::edge_t::falling);

    std::function<void(int)> fn =
        [&](int fucking) -> void { std::cout << "ah" << fucking << std::endl; };



    // lpl::epoll::epoll epoll(gpio.value_fd());

    // lpl::soft_pwm pwm(149);
    // pwm.enable();
    boost::asio::io_context io_ctx;
    boost::asio::ip::tcp::socket sock(io_ctx);
    std::array<int, 256> buf;
    sock.async_read_some(boost::asio::buffer(buf), [&](boost::system::error_code err, size_t s) {
        

        });

    while (true)
    {
        // epoll.wait();
        // std::cout << static_cast<int>(gpio.read()) - 48 << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }


    return 0;
}