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

#include <cstddef> //std::size_t
#include <iostream>
#include <stdexcept>
#include <utility> //std::as_const()
#include <vector>
#include "../header/color_models.h"
#include "../header/pixel.h"

/** CHANGE DESCRIPTION Image is an image that can be printed onto the screen, e.g. to the console output
	* - Pixel values are represented by a PxQ "matrix" (vectors) of "Pixel" objects for a PxQ image (P,Q: no. of pixels).
	* - Data can be loaded into the image
	*- The image can be printed to the set output using the method show()
	* @tparam T1 Color model for this image (default: Mono).
	* @tparam T2 Type of symbols that represent a Pixel (Default: char).
	*/

template <typename T1 = Mono, typename T2 = char>
class Image
{
private:

	/**
		* Aspect ratio = width/height of the image. The aspect ratio is only used in the output.
		* The true resulting value in the output is limited due to the coarse "pixel resolution" of the console output.
		*/
	float mAspectRatio = 1.;

	/**
		* Determines if space are filled that arise from mAspectRatio != 1, due to which the image is "stretched".
		*/
	bool mFillSpaces = false;

	/**
		* All Pixels that form the image in row-major order, i.e. mPixels.at(i) is the ith row of Pixels in the image.
		*/
	std::vector< std::vector< Pixel<T1,T2> > > mPixels;

	int mWidth; /*!< Width of the image in number of Pixels. */
	int mHeight; /*!< Height of the image in number of Pixels. */

	/**
		* Check if (i,j) is a valid coordinate inside the image.
		* @throw std::out_of_range if (i,j) lies outside the image
		*/
	virtual void verifyCoordinate(std::size_t i, std::size_t j) const;

public:
	/*Constructors and destructors*/
	Image(); /**< Constructs an empty image of size 1 x 1.*/
	Image(std::size_t width, std::size_t height); /**< Constructs an empty image of size width x height.*/
	Image(std::size_t width, std::size_t height, const Pixel<T1,T2> &p); /**< Constructs an image of size width x height with all pixels set equal to p.*/
	virtual ~Image() = default;

	/*Getters and setters*/

	/**
		* Get the Pixel at a specific coordinate in the image.
		* @param i Horizontal coordinate
		* @param j Vertical coordinate
		* @throw std::out_of_range if (i,j) lies outside the image.
		*/
	virtual const Pixel<T1,T2>& at( std::size_t i, std::size_t j ) const;

	/**
		* Get the Pixel at a specific coordinate in the image.
		* @param i Horizontal coordinate
		* @param j Vertical coordinate
		* @return (Non-const) reference to the Pixel
		* @throw std::out_of_range if (i,j) lies outside the image.
		*/
	virtual Pixel<T1,T2>& at( std::size_t i, std::size_t j );

	/**
		* Get the current aspect ratio
		* @return Aspect ratio
		*/
	virtual float getAspectRatio() const;

	/**
		* Set aspect ratio
		* @param aspectRatio as "width/height". The true resulting value in the output is limited due to the coarse "pixel resolution" of the console output.
		*/
	virtual void setAspectRatio( float aspectRatio );

	/**
		* Get all Pixels in the image.
		* @return Vector of vectors of all pixels in row-major order, i.e. getPixels().at(i) is the ith row of Pixels in the image.
		*/
	virtual const std::vector< std::vector< Pixel<T1,T2> > >& getPixels() const;

	/**
		* Fill the whole image with identical Pixels.
		* @param p Pixel that all Pixels in the image will be set equal to.
		*/
	virtual void setPixels( const Pixel<T1,T2> &p );

	/**
		* Get the current value of the option "FillSpaces"
		* @return True: Spaces arising from an aspect ratio not equal to one will be filled with the respective Pixels to the left; false: spaces are left blank.
		*/
	virtual bool getFillSpaces() const;

	/**
		* Set the value of the option "FillSpaces"
		* @param fillSpaces True: Space arising from an aspect ratio not equal to one will be filled with the respective Pixels to the left; false: spaces are left blank.
		*/
	virtual void setFillSpaces( bool fillSpaces );

	/**
		* Get height of the image.
		* @return height of the image in pixels
		*/
	virtual std::size_t getHeight() const;

	/**
		* Set height of the image.
		* @param height Height of the image in pixels
		*/
	virtual void setHeight( std::size_t height );

	/**
		* Get width of the image.
		* @return width of the image in pixels
		*/
	virtual std::size_t getWidth() const;

	/**
		* Set width of the image.
		* @param width Width of the image in pixels
		*/
	virtual void setWidth( std::size_t width );

	/**
		* Output the image to the command line.
		*/
	virtual void show() const;
};

/*Include method definitions for the template class*/
#include "../src/image.cpp"
