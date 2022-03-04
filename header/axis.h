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

/** A DESCRIPTION HERE
	*/
/* Axis represents an axis in a plot associated to it */
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

	//TODO: The following four should possibly better be maps or sets in order to improve lookups!
	std::vector<ImageCoordinate> mAllCoordinates; /*!< All ImageCoordinates in mPlot where Pixels of the axis/label/ticks are shown. */
	std::vector<ImageCoordinate> mAxisCoordinates; /*!< All ImageCoordinates in mPlot where Pixels of the axis itself (i.e. mAxisElement) are shown. */
	std::vector<ImageCoordinate> mLabelCoordinates; /*!< All ImageCoordinates in mPlot where Pixels of the label are shown. */
	std::vector<ImageCoordinate> mTicksCoordinates; /*!< All ImageCoordinates in mPlot where Pixels of the ticks are shown. */

	std::pair<int,int> mAddAttribute( const std::vector< Pixel<T,std::string> >& attribute, float position, std::size_t offsetFromAxis, bool leftOrBottom, std::vector<ImageCoordinate>& coordinateVector , bool rotate = false );
	void mAddRangeToCoordinateVector( std::vector<ImageCoordinate>& coordinateVector, const ImageCoordinate& firstNewCoordinate, std::size_t hRange, std::size_t vRange );
	void mShift( int horizontalShift, int verticalShift, std::vector<ImageCoordinate>& coordinateVector ); //Shift all indices in all the indexVectors horizontally by horizontalShift and vertically by verticalShift
	void mUpdateAllCoordinates(); //Updates the data member mAllCoordinates (e.g. after mAxisCoordinates etc. have been changed)

public:
	/*Constructors and destructors*/
	//Axis() = delete; //No default constructor because an Axis always needs an associated Plot2D in which it lives
	Axis( const Plot2D<T>& associatedPlot, const std::size_t width, const std::size_t height, const ImageCoordinate startIndex );

	/*---Getters and Setters----*/
	const Pixel<T,std::string>& at( const ImageCoordinate& idx ) const;
	const std::vector<ImageCoordinate>& getAxisCoordinates() const;
	const std::vector<ImageCoordinate>& getLabelCoordinates() const;
	const std::vector<ImageCoordinate>& getCoordinates() const;
	const Pixel<T,std::string>& getAxisElement() const;
	const std::size_t getLabelOffset() const;

	void setAxisElement( const Pixel<T,std::string>& axisElement);
	void setAxisColor( const T& color );
	void setAxisSymbol( const std::string& axisSymbol );

	void setLabelOffset( std::size_t labelOffset );
	/*--------------------------*/

	/*---------Modifiers--------*/
	void addLabel( const std::string& label, float position, bool leftOrBottom = true, bool rotateLabel = false ); //adds a label at position (0<=position<=1) relative to leftmost (horizontal axis) or top (vertical axis) pixel
	void addTicks( const std::vector<Tick>& ticks, bool leftOrBottom = true ); //adds the ticks "ticks.at(j).first" at relative positions "ticks.at(j).second"

	/*--------------------------*/

};

/*Include method definitions for the template class*/
#include "../src/axis.cpp"
