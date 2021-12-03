#include <cstring>
#include <iostream>
#include <thread>

#include <lpl/peripheral/uart.hpp>

int main(int argc, char** argv)
{
    lpl::uart::uart uart("/dev/ttyAMA0", lpl::uart::baudrates::b115200);
    lpl::str8_t<256> string = { 0, };

    int count = 0;

    // while (count < 100)
    // {
    //     using namespace std::literals;
    //     auto res = uart.read(string, std::strlen(string));
    //     std::cout << string;
    //     std::this_thread::sleep_for(200ms);
    // }

    while (count < 10)
    {
        using namespace std::literals;
        std::sprintf(string, "hell world %d\r\n", 0);
        auto res = uart.write(string, std::strlen(string));
        std::cout << res << std::endl;
        std::this_thread::sleep_for(200ms);
    }

    return 0;
}
