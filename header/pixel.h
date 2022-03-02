#pragma once

/*Pixel represents a single "pixel" in an image.
- The pixel is represented by a symbol of Type T2 (e.g. char 'x') and a color of color model T1 (e.g. Mono with b = 0 -> black)
*/

#include <iostream>
#include <stdexcept>
#include "../header/color_models.h"

template <typename T1 = Mono, typename T2 = char> //T1: color model, T2: type of each pixel (e.g. char)
class Pixel
{
public:
	/*Constructors and destructors*/
	Pixel() = default; //Construct an empty image of size 1x1
	Pixel( T1 color, T2 symbol ); //Construct an empty image of size 1x1

	/*Getters and setters*/
	T1 getColor() const;
	void setColor( T1 color );
	T2 getSymbol() const;
	void setSymbol( T2 symbol );

private:
	T1 mColor; //Color of the pixel
	T2 mSymbol; //Symbol representing the pixel if displayed in the terminal
	void verifyCoordinate(std::size_t i, std::size_t j) const; //Verify that (i,j) is within limits of the image size (throws std::out_of_range() if not)
};

template <typename T1, typename T2>
std::ostream& operator<<( std::ostream& ostr, const Pixel<T1,T2>& p ); //Outputs the (colored) pixel p to the stream ostr

/*Include method definitions (needed here for the compiler because it is a class template!)*/
#include "../src/pixel.cpp"
