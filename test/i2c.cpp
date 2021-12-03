#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>

#include <etl/crc16.h>

#include <lpl/util.hpp>
#include <lpl/peripheral/i2c.hpp>

namespace lcd
{

constexpr uint8_t i2c_addr = 0x27;
constexpr size_t lcd_width = 16;

constexpr uint8_t lcd_chr = 1;
constexpr uint8_t lcd_cmd = 0;

constexpr uint8_t lcd_backlight = 0x08;

constexpr uint8_t enable = 0b00000100;

const lpl::arr_t<uint8_t, 4> lines = { 0x80, 0xc0, 0x94, 0xd4 };

void toggle_enable(lpl::i2c::i2c& clcd, uint8_t data)
{
    std::chrono::seconds(1);
    using namespace std::literals;
    lpl::realtime::delay_for(1ms);
    clcd.write((data | enable));
    lpl::realtime::delay_for(1ms);
    clcd.write((data & ~enable));
    lpl::realtime::delay_for(1ms);
}

void write_byte(lpl::i2c::i2c& clcd, uint8_t byte, uint8_t mode)
{
    uint8_t bits_high = mode | (byte & 0xF0) | lcd_backlight;
    uint8_t bits_low = mode | (byte << 4 & 0xF0) | lcd_backlight;

    clcd.write(bits_high);
    toggle_enable(clcd, bits_high);

    clcd.write(bits_low);
    toggle_enable(clcd, bits_low);
}

void init(lpl::i2c::i2c& clcd)
{
    write_byte(clcd, 0x33, lcd_cmd); // 110011 Initialise
    write_byte(clcd, 0x32, lcd_cmd); // 110010 Initialise
    write_byte(clcd, 0x06, lcd_cmd); // 000110 Cursor move direction
    write_byte(clcd, 0x0C, lcd_cmd); // 001100 Display On,Cursor Off, Blink Off 
    write_byte(clcd, 0x28, lcd_cmd); // 101000 Data length, number of lines, font size
    write_byte(clcd, 0x01, lcd_cmd); // 000001 Clear display
    write_byte(clcd, 0x02, lcd_cmd); // 000010 Return home
}

void write_string(lpl::i2c::i2c& clcd, const char* string, uint8_t line)
{
    using namespace std::literals;
    write_byte(clcd, line, lcd_cmd);

    for (int i = 0; i < lcd_width; i++)
    {
        write_byte(clcd, static_cast<uint8_t>(string[i]), lcd_chr);
    }
}

}


namespace temphumi // AM2315 sensor
{

constexpr uint8_t i2c_addr = 0x5c;

constexpr uint8_t read_reg = 0x03;

constexpr uint8_t config_shutdown = 0b00000001;
constexpr uint8_t config_oneshot = 0b10000000;

struct temphumi_t
{
    float temperature;
    float humidity;
};

void init(lpl::i2c::i2c& temphumi)
{
    using namespace std::literals;
    // wake up
    temphumi.write(0x00);
    lpl::realtime::delay_for(2ms);
}

uint16_t get_crc(uint8_t* buf, size_t size)
{
    return 0;
}

temphumi_t read(lpl::i2c::i2c& temphumi)
{
    using namespace std::literals;
    temphumi_t val{};

    lpl::arr_t<uint8_t, 3> req = { read_reg, 0x00, 0x04 };
    temphumi.write(req, 3);

    lpl::realtime::delay_for(3ms);

    lpl::arr_t<uint8_t, 8> buf = { 0, };

    auto read_bytes = temphumi.read(buf, 8);
    
    uint16_t read_crc = (static_cast<uint16_t>(buf[6]) << 8) | buf[7];
    etl::crc16_t16 crc16;
    for (int i = 0; i < 6; i++)
        crc16.add(buf[i]);
    
    val.humidity = static_cast<uint16_t>(buf[2]) << 8;
    val.humidity += buf[3];
    val.humidity /= 10;

    val.temperature = static_cast<uint16_t>(buf[4]  & 0x7F) << 8;
    val.temperature += buf[5];
    val.temperature /= 10;

    if (buf[4] >> 7) val.temperature = -val.temperature;

    return val;    
}

}

int main(int argc, char **argv)
{
    using namespace std::literals;
    
    lpl::i2c::i2c char_lcd(0, lcd::i2c_addr);
    const char string_1[17] = "Hell I2C CLCD!!!";
    const char string_2[17] = "I am JaehwanJin!";
    lcd::init(char_lcd);
    lcd::write_string(char_lcd, string_1, lcd::lines[0]);
    lcd::write_string(char_lcd, string_2, lcd::lines[1]);

    lpl::i2c::i2c thermo_sensor(0, temphumi::i2c_addr);
    temphumi::init(thermo_sensor);
    for (int i = 0; i < 10; i++)
    {
        auto th = temphumi::read(thermo_sensor);
        std::cout << std::fixed << std::setprecision(3)
                  << "temperature : " << th.temperature
                  << ", humidity : " << th.humidity
                  << std::endl;
    }

    return 0;
}