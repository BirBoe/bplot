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

using ImageCoordinate = std::pair<std::size_t,std::size_t>;

/**
	* The Image class represents an image that can be printed to the console (or another output stream).
	* The Image is a collection of Pixel objects that are ordered in a width x height "matrix".
	* The Image can be either black and white (Mono) or colored (RGB).
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
		* Checks if an ImageCoordinate is a valid coordinate inside the image.
		* @param coord ImageCoordinate to be verified: pair of horizontal and vertical indices
		* @throw std::out_of_range if ImageCoordinate lies outside the image
		*/
	virtual void verifyCoordinate( const ImageCoordinate& coord ) const;

public:
	/*----Constructors and destructors----*/

	Image(); /**< Constructs an empty image (of default Pixels) of size 1 x 1.*/

	/**
		* Constructs an empty image (of default Pixels) of given size.
		* @param width Width of the Image in number of Pixels.
		* @param height Height of the Image in number of Pixels.
		*/
	Image( std::size_t width, std::size_t height );

	/**
		* Constructs an empty image of given size with identical Pixels.
		* @param width Width of the Image in number of Pixels.
		* @param height Height of the Image in number of Pixels.
		* @param p Pixel that all Pixels in the Image will be set equal to.
		*/
	Image( std::size_t width, std::size_t height, const Pixel<T1,T2> &p );
	virtual ~Image() = default;

	/*------------------------------------*/

	/*----Getters and setters----*/

	/**
		* Get the Pixel at a specific coordinate in the image.
		* @param coord ImageCoordinate: pair of horizontal and vertical indices
		* @return Reference to the Pixel at coord
		* @throw std::out_of_range if coord lies outside the image.
		*/
	virtual const Pixel<T1,T2>& at( const ImageCoordinate& coord ) const;

	/**
		* Get the Pixel at a specific coordinate in the image.
		* @param coord ImageCoordinate: pair of horizontal and vertical indices
		* @return (Non-const) reference to the Pixel at coord
		* @throw std::out_of_range if coord lies outside the image.
		*/
	virtual Pixel<T1,T2>& at( const ImageCoordinate& coord );

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
	virtual void setAllPixels( const Pixel<T1,T2> &p );

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

	/*--------------------------*/

	/*----Other Methods----*/

	/**
		* Output the image to the command line.
		*/
	virtual void show() const;

	/**
		* Refreshes the image displayed in the command line (i.e. show has been called before):
		* The method assumes that the command-line cursor is at the same position
		* it would be after calling show(). Based on this assumption, the cursor is set back
		* to the upper left Pixel of the image and the "old" image is overwritten with a new
		* call to show. Assumes that the image width and height have not changed.
		* @see refresh( std::size_t oldWidth, std::size_t oldHeight )
		*/
	virtual void refresh() const;

	/**
		* Refreshes the image displayed in the command line (i.e. show has been called before):
		* The method assumes that the command-line cursor is at the same position
		* it would be after calling show(). Based on this assumption, the cursor is set back
		* to the upper left Pixel of the image and the "old" image is overwritten with a new
		* call to show.
		* @param oldHeight Height of the previous version of the image that was output with show()
		*/
	virtual void refresh( std::size_t oldHeight ) const;

	/*--------------------------*/
};

/*Include method definitions for the template class*/
#include "../src/image.cpp"
