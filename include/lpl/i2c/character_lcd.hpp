#ifndef __LPL_I2C_CHARACTER_LCD_HPP___
#define __LPL_I2C_CHARACTER_LCD_HPP___

#include <etl/string.h>

#include "../peripheral/i2c.hpp"

namespace lpl
{
namespace i2c
{

template <size_t col, size_t row>
class character_lcd
{

public:
	character_lcd();

	void clear();

	void set_cursor(bool onoff);
	void set_cursor_pos(int x, int y);

	void write(const uint8_t* value, size_t size);
	void write(const char* value);

	template <size_t _MaxSize>
	void write(const etl::string<_MaxSize>& value);

};


using character_lcd_16x2 = character_lcd<16, 2>;
using character_lcd_16x4 = character_lcd<16, 4>;
using character_lcd_20x4 = character_lcd<20, 4>;


}
}



#endif