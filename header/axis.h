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

#include <algorithm> //std::find, std::max
#include <string>
#include <utility> //std::pair

#include "../header/color_models.h"
#include "../header/pixel.h"

//Forward declaration of Plot2D, since it is needed as reference member in the Axis class
template <typename T>
class Plot2D;

using ImageCoordinate = std::pair<std::size_t,std::size_t>; //Stores a pair (jh,jv) of horizontal and vertical indices in the base image of the plot
using Tick = std::pair<std::string,float>; //Stores a "Tick" next to the axis in the form (TickLabel, relativePosition)

/**
	* The Axis class represents a (horizontal or vertical) axis that is part of a Plot2D.
	* The Axis consists of elements of the axis itself (e.g. "_" or "|") and may have labels ("x axis") and ticks ("1 2 3 4") to it.
	* An Axis object cannot exists on its own but is always associated to a Plot2D.
	* @tparam T1 Color model for this image (default: Mono).
	* @tparam T2 Type of symbols that represent a Pixel (Default: char).
	*/

template <typename T = Mono>
class Axis
{
private:
	inline static const Pixel<T,std::string> emptyPixel = Pixel<T,std::string>(T()," ");

	const Plot2D<T> &mPlot; /*!< Plot that this axis is part of. */

	ImageCoordinate mUpperLeftIndex; /*!< Upper left index in mPlot of the rectangle holding the axis including ticks and labels. */
	ImageCoordinate mLowerRightIndex; /*!< Lower right index in mPlot of the rectangle holding the axis including ticks and labels. */

	bool mIsHorizontal; /*!< True: This is a horitontal axis. False: This is a vertical axis. */
	Pixel<T,std::string> mAxisElement; /*!< Pixel representing a single element of the axis. */
	std::vector< Pixel<T,std::string> > mLabel; /*!< Pixels holding the label of the axis. Their positions in mPlot are stored in mLabelCoordinates. */
	std::vector< Pixel<T,std::string> > mTicks; /*!< Pixels holding the ticks of the axis. Their positions in mPlot are stored in mTicksCoordinates. */
	std::size_t mLabelOffset = 1; /*!< Space (in number of Pixels) between label and axis. */

	//TODO: The following four should probably better be maps or sets in order to improve lookups!
	std::vector<ImageCoordinate> mAllCoordinates; /*!< All ImageCoordinates in mPlot where Pixels of the axis/label/ticks are shown. */
	std::vector<ImageCoordinate> mAxisCoordinates; /*!< All ImageCoordinates in mPlot where Pixels of the axis itself (i.e. mAxisElement) are shown. */
	std::vector<ImageCoordinate> mLabelCoordinates; /*!< All ImageCoordinates in mPlot where Pixels of the label are shown. */
	std::vector<ImageCoordinate> mTicksCoordinates; /*!< All ImageCoordinates in mPlot where Pixels of the ticks are shown. */

	/**
		* Adds an attribute (i.e. any collection of Pixels) at a fixed relative position along the axis and stores the coordinates.
		* @param attribute Attribute that will be added to the axis
		* @param position Relative position (between 0 and 1, otherwise set to 0) along the axis where the first pixel of attribute is placed
		* @param offsetFromAxis Number of empty pixels between the axis itself and the attribute
		* @param leftOrBelow True: The attribute is placed to the left of the axis (if axis is vertical) or below it (if axis is horizontal).
		*											Otherwise it is placed to the right or above.
		* @param coordinateVector Vector of ImageCoordinates to which the ImageCoordinates of the attribute will be added.
		* @param rotate True: The attribute is rotated if the axis is vertical, i.e. it will extend vertically instead of horizontally.
		* @return A pair of horizontal and vertical number of Pixels that all Pixels belonging to other parts of the axis
		* 				have to be moved to make space for the attribute. The shifts are not performed within the method.
		* @todo The method is too long and should be decomposed in a useful way!
		*/
	std::pair<int,int> mAddAttribute( const std::vector< Pixel<T,std::string> >& attribute, float position, std::size_t offsetFromAxis, bool leftOrBelow, std::vector<ImageCoordinate>& coordinateVector , bool rotate = false );

	/**
		* Adds a range of ImageCoordinates to a vector of ImageCoordinates
		* @param coordinateVector Vector to which coordinates will be added.
		* @param firstNewCoordinate First of the coordinates that will be added, i.e. beginning of the range.
		* @param hRange Horizontal extension of the range
		* @param vRange Vertical extension of the range
		*/
	void mAddRangeToCoordinateVector( std::vector<ImageCoordinate>& coordinateVector, const ImageCoordinate& firstNewCoordinate, std::size_t hRange, std::size_t vRange );

	/**
		* Shifts all ImageCoordinates in a vector by given number of Pixels
		* @param coordinateVector Vector of coordinates that will be shifted
		* @param horizontalShift Horizontal shift in number of Pixels
		* @param verticalShift Vertical shift in number of Pixels
		* @todo What happens if size_t becomes negative somewhere?
		*/
	void mShift( std::vector<ImageCoordinate>& coordinateVector, int horizontalShift, int verticalShift );

	/**
		* Updates the vector of all ImageCoordinates of the axis (mAllCoordinates). Has to be called after any other coordinate vector is changed.
		*/
	void mUpdateAllCoordinates(); //Updates the data member mAllCoordinates (e.g. after mAxisCoordinates etc. have been changed)

public:
	/*----Constructors and destructors----*/
	//Axis() = delete; //No default constructor because an Axis always needs an associated Plot2D in which it lives

	/**
		* Constructs a (horizontal or vertical) Axis of given size that is part of a given plot.
		* @param associatedPlot Plot that the axis is part of.
		* @param width Width of the axis in number of Pixels.
		* @param height Height of the axis in number of Pixels. If height>width the axis is a "vertical" axis, otherwise a "horizontal" axis.
		* @param startCoordinate First coordinate of the axis from which the axis extends in the plot.
		*/
	Axis( const Plot2D<T>& associatedPlot, const std::size_t width, const std::size_t height, const ImageCoordinate startCoordinate );

	/*------------------------------------*/

	/*--------Getters and Setters--------*/
	/**
		* Get the Pixel at a specific coordinate in the plot of which the axis is a part.
		* @param coord Coordinate in the plot of which the axis is a part.
		* @return Pixel at the given coordinate. If no Pixel of the axis/label/ticks exists at the coord, an emptyPixel is returned.
		*/
	const Pixel<T,std::string>& at( const ImageCoordinate& coord ) const;

	/**
		* Get all coordinates in the plot of which the axis is part where Pixels of the axis itself are shown.
		* @return Vector of all ImageCoordinates of axis elements.
		*/
	const std::vector<ImageCoordinate>& getAxisCoordinates() const;

	/**
		* Get all coordinates in the plot of which the axis is part where Pixels of the label are shown.
		* @return Vector of all ImageCoordinates of parts of the label.
		*/
	const std::vector<ImageCoordinate>& getLabelCoordinates() const;

	/**
		* Get all coordinates in the plot of which the axis is part where Pixels of the axis/label/ticks are shown.
		* @return Vector of all ImageCoordinates of parts of the axis/label/ticks
		*/
	const std::vector<ImageCoordinate>& getCoordinates() const;

	/**
		* Get the Pixel that represents an element of the axis itself.
		* @return Pixel that is an element of the axis.
		*/
	const Pixel<T,std::string>& getAxisElement() const;

	/**
		* Set the Pixel that represents an element of the axis itself.
		* @param axisElement Pixel out of which the entire axis will be constructed.
		* @see setAxisColor( const T& color )
		* @see setAxisSymbol( const std::string& axisSymbol )
		*/
	void setAxisElement( const Pixel<T,std::string>& axisElement);

	/**
		* Set the Color of the axis.
		* @param color Color of the axis element (not label and ticks).
		*/
	void setAxisColor( const T& color );

	/**
		* Set the Symbol that represents an element of the axis itself.
		* @param axisSymbol Symbol out of which the entire axis will be constructed.
		*/
	void setAxisSymbol( const std::string& axisSymbol );

	/**
		* Get the offset between axis and label (if any).
		* @return Number of Pixels between axis itself and the label. Default value: 1
		*/
	const std::size_t getLabelOffset() const;

	/**
		* Set the offset between axis and label (if any).
		* @param labelOffset Number of Pixels between axis itself and the label. Default value: 1
		*/
	void setLabelOffset( std::size_t labelOffset );

	/*--------------------------*/

	/*---------Modifiers--------*/

	/**
		* Adds a label to the axis at a fixed relative position. Does nothing, if another label was added before.
		* @param label Label of the axis.
		* @param position Relative position (between 0 and 1, otherwise set to 0) along the axis where the first character of the label is placed.
		* @param leftOrBelow True: The label is placed to the left of the axis (if axis is vertical) or below it (if axis is horizontal).
		*											Otherwise it is placed to the right or above.
		* @param rotateLabel True: The label is rotated if the axis is vertical, i.e. it will extend vertically instead of horizontally.
		* @todo Should ticks be shifted to make space for the label
		*/
	void addLabel( const std::string& label, float position, bool leftOrBelow = true, bool rotateLabel = false );

	/**
		* Adds ticks to the axis.
		* @param ticks Vector of ticks that will be added (tick: pair of string (its label) and float (its relative position, between 0 and 1, otherwise set to 0).
		* @param leftOrBelow True: The ticks are placed to the left of the axis (if axis is vertical) or below it (if axis is horizontal).
		*											Otherwise they are placed to the right or above.
		* @todo Perform the axis shift in a better way so that ticks of different length work correctly.
		*/
	void addTicks( const std::vector<Tick>& ticks, bool leftOrBelow = true );

	/*--------------------------*/

};

/*Include method definitions for the template class*/
#include "../src/axis.cpp"
