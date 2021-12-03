#include <lpl/util.hpp>
#include <lpl/peripheral/led.hpp>

int main(int argc, char** argv)
{
    using namespace std::literals;
    lpl::led::led led("pwr");
    
    int val = 0;

    for (int i = 0; i < 5; i++)
    {
        val = !val;
        led.set_brightness(val);
        lpl::realtime::delay_for(500ms);
    }

    return 0;
}