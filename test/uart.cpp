#include <cstring>
#include <iostream>
#include <thread>

#include <etl/array.h>

#include <lpl/peripheral/uart.hpp>

int main(int argc, char** argv)
{
    lpl::uart::uart uart("/dev/ttyS0", lpl::uart::baudrates::b115200);
    etl::array<lpl::str8_t<256>, 5> strs;

    for (auto* str : strs)
    {
        using namespace std::literals;
        std::memset(str, 0, 256);
        auto res = uart.read(str, 256);
        std::cout << res << std::endl;
        std::cout << str;
    }


    for (auto* str : strs)
    {
        using namespace std::literals;
        auto res = uart.write(str, std::strlen(str));
        std::cout << res << std::endl;
        std::this_thread::sleep_for(200ms);
    }

    return 0;
}
