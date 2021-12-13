#include <iostream>
#include <lpl/peripheral/spi.hpp>

#include <boost/filesystem/path.hpp>

int main(int argc, char** argv)
{
    lpl::spi::spi icm20948(0, 0, 400'000, lpl::spi::modes::mode_0);

    lpl::arr_t<uint8_t, 256> tx_buf = { 0, };
    lpl::arr_t<uint8_t, 256> rx_buf = { 0, };

    icm20948.write_full(tx_buf, 1, rx_buf, 256);

    std::cout << "ah";
    
    return 0;
}