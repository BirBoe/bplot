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
