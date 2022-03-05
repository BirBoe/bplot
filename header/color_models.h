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

#pragma once
#include <cstdint> //defines integral types int8_t etc.

/**
	* RGB color model (8-bit)
	*/
struct RGB {
	uint8_t r = 255; /*!< red value*/
	uint8_t g = 255; /*!< green value*/
	uint8_t b = 255; /*!< blue value*/
};

/**
	* Get the ansi color code for an RGB color, e.g. to be used in output to the console
	* @param color RGB color to be "translated"
	* @return ansi color code
	*/
std::string color_to_ansi( RGB color ) {
	return "\033[38;2;" + std::to_string(color.r) + ";" + std::to_string(color.g) + ";" + std::to_string(color.b) + "m";
}

/**
	* Mono color model (1-bit): black or white
	*/
struct Mono {
	bool b = 1; /*!< black value*/
};

/**
	* Get the ansi color code for a Mono color, e.g. to be used in output to the console
	* @param color Mono color to be "translated"
	* @return ansi color code
	*/
std::string color_to_ansi( Mono color ) {
	if( color.b == 1 ) return "\033[30m";
	if( color.b == 0 ) return "\033[37m";
}
