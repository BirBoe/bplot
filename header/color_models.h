/* bplot - a tool to plot two-dimensional data to the command line
 * Copyright (C) 2022  Birger Böning
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*Defines structs that represent color values for single pixels*/

#pragma once
#include <cstdint> //defines integral types int8_t etc.

//RGB (8-bit) color scale
struct RGB {
	uint8_t r = 255;
	uint8_t g = 255;
	uint8_t b = 255;
};

std::string color_to_ansi( RGB color ) {
	return "\033[38;2;" + std::to_string(color.r) + ";" + std::to_string(color.g) + ";" + std::to_string(color.b) + "m";
}

//Mono color scale (binary values)
struct Mono {
	bool b = 1; //b=1: black, b=0: white
};

std::string color_to_ansi( Mono color ) {
	if( color.b == 1 ) return "\033[30m";
	if( color.b == 0 ) return "\033[37m";
}
