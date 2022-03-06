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

#include <iostream>
#include <stdexcept>
#include "../header/color_models.h"

/**
	* The Pixel class represents the smallest unit of an Image and as such can be printed to the console (or another outpus stream).
	* A Pixel is represented as a symbol (for example a char, string or int) of a given color (Mono of RGB).
	* @tparam T1 Color model for this Pixel (default: Mono).
	* @tparam T2 Type of symbols that represent the Pixel in the command line (Default: char).
	*/

template <typename T1 = Mono, typename T2 = char>
class Pixel
{
public:
	/*----Constructors and destructors----*/

	Pixel() = default; /*!< Constructs a Pixel with default values for the color model and the symbol type used */

	/**
	  * Constructs a Pixel as symbol of given color
		* @param color Color of the Pixel.
		* @param symbol Symbol that is represents the Pixel in the command line.
		*/
	Pixel( T1 color, T2 symbol );

	/*-----------------------------------*/

	/*----Getters and setters----*/
	
	/**
	  * Get color of the Pixel
		* @return Color of the Pixel
		*/
	T1 getColor() const;

	/**
	  * Set color of the Pixel
		* @param color Color of the Pixel
		*/
	void setColor( T1 color );

	/**
	  * Get symbol that represents the Pixel in the command line.
		* @return Symbol of the Pixel
		*/
	T2 getSymbol() const;

	/**
	  * Set symbol that represents the Pixel in the command line.
		* @param symbol Symbol of the Pixel
		*/
	void setSymbol( T2 symbol );

	/*--------------------------*/

private:
	T1 mColor; /*!< Color of the pixel */
	T2 mSymbol;  /*!< Symbol that is shown if the pixel is printed to the command line */
};


/**
	* Print a Pixel to an output stream using ansi codes to set its color.
	*/
template <typename T1, typename T2>
std::ostream& operator<<( std::ostream& ostr, const Pixel<T1,T2>& p ); //Outputs the (colored) pixel p to the stream ostr

/*Include method definitions for the template class*/
#include "../src/pixel.cpp"
