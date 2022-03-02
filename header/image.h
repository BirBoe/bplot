#pragma once

#include <cstddef> //std::size_t
#include <iostream>
#include <stdexcept>
#include <utility> //std::as_const()
#include <vector>
#include "../header/color_models.h"
#include "../header/pixel.h"

/** Image is an image that can be printed onto the screen, e.g. to the console output
	* - Pixel values are represented by a PxQ "matrix" (vectors) of "Pixel" objects for a PxQ image (P,Q: no. of pixels).
	* - Data can be loaded into the image
	*- The image can be printed to the set output using the method show()
	*/

template <typename T1 = Mono, typename T2 = char> //T1: color model, T2: type of each pixel (e.g. char)
class Image
{
public:
	/*Constructors and destructors*/
	Image(); /**< Constructs an empty image of size 1 x 1.*/
	Image(std::size_t width, std::size_t height); /**< Constructs an empty image of size width x height.*/
	Image(std::size_t width, std::size_t height, const Pixel<T1,T2> &p); /**< Constructs an image of size width x height with all pixels set equal to p.*/
	virtual ~Image() = default;

	/*Getters and setters*/
	virtual float getAspectRatio() const;
	virtual void setAspectRatio( float aspectRatio );
	virtual const Pixel<T1,T2>& at( std::size_t i, std::size_t j ) const; //Returns the Pixel at position (i,j) as const reference
	virtual Pixel<T1,T2>& at( std::size_t i, std::size_t j ); //Returns the Pixel at position (i,j) as non-const reference
	virtual const std::vector< std::vector< Pixel<T1,T2> > >& getPixels() const;//Return a const reference to the row major matrix of pixels
	virtual void setPixels( const Pixel<T1,T2> &p ); //Set all pixels in the image equal to p
	virtual bool getFillSpaces() const;
	virtual void setFillSpaces( bool fillSpaces );
	virtual std::size_t getHeight() const;
	virtual void setHeight( std::size_t height );
	virtual std::size_t getWidth() const;
	virtual void setWidth( std::size_t width );

	/*Output*/
	virtual void show() const; //Show the image in the terminal

private:
	float mAspectRatio = 1.; //AspectRatio of the figure (so far: rounded to nearest int!). The aspect ratio is only used in the output. Note: The real aspect ratio in the output depends on the terminal simulator in use.
	bool mFillSpaces = false; //If true and mAspectRatio != 1.: spaces are filled with symbol to the left. Otherwise spaces are left blank.
	std::vector< std::vector< Pixel<T1,T2> > > mPixels; //row major matrix of pixels: pixel = mPixels.at(iRow).at(iColumn)
	int mWidth, mHeight; //Dimensions of the image in numbers of pixels (independent of mAspectRatio!)
	virtual void verifyCoordinate(std::size_t i, std::size_t j) const; //Verify that (i,j) is within limits of the image size (throws std::out_of_range() if not)
};

/*Include method definitions (needed here for the compiler because it is a class template!)*/
#include "../src/image.cpp"
