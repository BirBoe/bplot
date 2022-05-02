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

#include "../header/plot2d.h"

/*-------------------Private member functions-----------------------*/

template <typename T>
void Axis<T>::mAddRangeToCoordinateVector( std::vector<ImageCoordinate>& coordinateVector, const ImageCoordinate& firstNewCoordinate, std::size_t hRange, std::size_t vRange )
{
	std::size_t firstIndex = coordinateVector.size();
	coordinateVector.resize( coordinateVector.size() + hRange*vRange );

	//Add the range of coordinates to the coordinateVector in row-major order
	std::size_t hCoord = firstNewCoordinate.first;
	std::size_t vCoord = firstNewCoordinate.second;
	for( std::size_t jv = 0; jv < vRange; jv++ ) {
		vCoord++;
		if( vCoord >= mPlot.getHeight() ) vCoord = mPlot.getHeight() - 1;
		for( std::size_t jh = 0; jh < hRange; jh++) {
			hCoord++;
			if( hCoord >= mPlot.getWidth() ) hCoord = mPlot.getWidth() - 1;
			coordinateVector.at(firstIndex + jh + jv * hRange) = std::make_pair( hCoord, vCoord );
			//coordinateVector.at(firstIndex + jh + jv * hRange) = std::make_pair( firstNewCoordinate.first + jh, firstNewCoordinate.second + jv );
		}
	}
}

template <typename T>
std::pair<int,int> Axis<T>::mAddAttribute( const std::vector< Pixel<T,std::string> >& attribute, float position, std::size_t offsetFromAxis, bool leftOrBelow, std::vector<ImageCoordinate>& coordinateVector , bool rotate )
{
	if( position > 1. || position < 0. ) position = 0.;

	int hTotalOffset = 0, vTotalOffset = 0; //Horizontal and vertical shifts that the method will return as a pair
	ImageCoordinate startCoordinate = mAxisCoordinates.at(0);

	//If parts of the axis, labels, etc. go out of the coordinate range of the plot, they will be shifted back to within the range by overallShift
	int overallShift = 0;

	if( mIsHorizontal ) {

		std::size_t absolutePosition = static_cast<std::size_t>( position * (mLowerRightIndex.first - mUpperLeftIndex.first + 1) );

		if( !leftOrBelow ) { //Label is above the axis

			if( startCoordinate.second  < offsetFromAxis ) { //If axis and attribute will not fit inside the plot...
				overallShift = offsetFromAxis - startCoordinate.second; //...they will be shifted vertically by this amount ( < 0, i.e. upwards )
			}

			//Compute the coordinate range required for the attribute and add it to the vector of coordinates
			mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startCoordinate.first + absolutePosition, startCoordinate.second - offsetFromAxis + overallShift ), attribute.size(), 1 );

			if( mUpperLeftIndex.second + offsetFromAxis > startCoordinate.second ) { //If the attribute does not fit inside the coordinate range of this axis...
				mUpperLeftIndex.second = startCoordinate.second - offsetFromAxis; //...change the boundary of the range.
			}

		} else { //Label is below the axis

			if( startCoordinate.second + offsetFromAxis >= mPlot.getHeight() ) { //If axis and attribute will not fit inside the plot...
				overallShift = mPlot.getHeight() - 1 - startCoordinate.second - offsetFromAxis; //...they will be shifted to the left by this amount ( > 0, i.e. downwards )
			}

			mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startCoordinate.first + absolutePosition, startCoordinate.second + offsetFromAxis + overallShift ), attribute.size(), 1 );

			if( mLowerRightIndex.second < startCoordinate.second + offsetFromAxis ) { //If the attribute does not fit inside the coordinate range of this axis...
				mLowerRightIndex.second = startCoordinate.second + offsetFromAxis; //...change the boundary of the range.
			}

		}

		//Apply the overallShift to the necessary vertical coordinates
		vTotalOffset += overallShift;
		mUpperLeftIndex.second += overallShift;
		mLowerRightIndex.second += overallShift;

	} else { //This is a vertical axis

		std::size_t absolutePosition = static_cast<std::size_t>( position * ( mLowerRightIndex.second - mUpperLeftIndex.second + 1 ) );

		if( leftOrBelow ) { //Attribute is left of the axis

			if( rotate && startCoordinate.first < offsetFromAxis ) { //If axis and the rotated attribute will not fit inside the plot...
				overallShift = offsetFromAxis - startCoordinate.first; //...they will be shifted horiontally by this amount ( < 0, i.e. to the left )
			}
			if( !rotate && startCoordinate.first < offsetFromAxis + attribute.size() ) { //If axis and the attribute will not fit inside the plot...
				overallShift = offsetFromAxis + attribute.size() - startCoordinate.first; //...they will be shifted horiontally by this amount ( > 0, i.e. to the right )
			}

			//Compute the coordinate range required for the attribute and add it to the vector of coordinates
			if(rotate) {
				mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startCoordinate.first - offsetFromAxis + overallShift, startCoordinate.second + absolutePosition ), 1, attribute.size() );
			} else {
				mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startCoordinate.first - offsetFromAxis + overallShift - 1, startCoordinate.second + absolutePosition ), attribute.size(), 1 );
			}

			//If the attribute is not rotated, space is required to accomodate the label:
			if( !rotate ) {
				hTotalOffset += attribute.size();
				offsetFromAxis += attribute.size();
			}

			if( mUpperLeftIndex.first + offsetFromAxis > startCoordinate.first ) { //If the attribute does not fit inside the coordinate range of this axis...
				mUpperLeftIndex.first = startCoordinate.first - offsetFromAxis; //...change the boundary of the range.
			}

		} else { //Attribute is right of the axis

			if( rotate && startCoordinate.first + offsetFromAxis >= mPlot.getWidth() ) { //If axis and the rotated attribute will not fit inside the plot...
				overallShift = mPlot.getWidth() - 1 - startCoordinate.first - offsetFromAxis; //...they will be shifted horiontally by this amount ( < 0, i.e. to the left )
			}
			if( !rotate && startCoordinate.first + offsetFromAxis + attribute.size() >= mPlot.getWidth() ) { //If axis and the attribute will not fit inside the plot...
				overallShift = mPlot.getWidth() - 1 - startCoordinate.first - offsetFromAxis - attribute.size(); //...they will be shifted horiontally by this amount ( > 0, i.e. to the right )
			}

			//Compute the coordinate range required for the attribute and add it to the vector of coordinates
			if(rotate) {
				mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startCoordinate.first + offsetFromAxis + overallShift, startCoordinate.second + absolutePosition ), 1, attribute.size() );
			} else {
				mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startCoordinate.first + offsetFromAxis + overallShift, startCoordinate.second + absolutePosition ), attribute.size(), 1 );
			}

			//If the attribute is not rotated, space is required to accomodate the label:
			if( !rotate ) {
				offsetFromAxis += attribute.size();
			}

			if( mLowerRightIndex.first < startCoordinate.first + offsetFromAxis ) { //If the attribute does not fit inside the coordinate range of this axis...
				mLowerRightIndex.first = startCoordinate.first + offsetFromAxis; //...change the boundary of the range.
			}

		}

		//Apply the overallShift to the necessary vertical coordinates
		hTotalOffset += overallShift;
		mUpperLeftIndex.first += overallShift;
		mLowerRightIndex.first += overallShift;

	}

	//Return the potentially necessary shifts, so that the caller of the method can apply them
	return std::make_pair(hTotalOffset,vTotalOffset);
}


template <typename T>
void Axis<T>::mShift( std::vector<ImageCoordinate>& coordinateVector, int horizontalShift, int verticalShift )
{
	for( auto& coord : coordinateVector ) {
		if( coord.first + horizontalShift < 0 || coord.second + verticalShift < 0 ) {
			throw std::out_of_range("Attempted shift of coordinates failed.");
		}
		coord.first += horizontalShift;
		coord.second += verticalShift;

		//Check if the shift would go out of bounds of the plot
		/*if( coord.first >=  mPlot.getWidth() ) coord.first = mPlot.getWidth() - 1;
		if( coord.first >=  mPlot.getHeight() ) coord.first = mPlot.getHeight() - 1;*/

		if( coord.first > mLowerRightIndex.first ) {
			mLowerRightIndex.first = coord.first;
		} else if( coord.first < mUpperLeftIndex.first ) {
			mUpperLeftIndex.first = coord.first;
		}
		if( coord.second > mLowerRightIndex.second ) {
			mLowerRightIndex.second = coord.second;
		} else if( coord.second < mUpperLeftIndex.second ) {
			mUpperLeftIndex.second = coord.second;
		}
	}
}

template <typename T>
void Axis<T>::mUpdateAllCoordinates()
{
	mAllCoordinates.clear();
	mAllCoordinates.reserve( ( mLowerRightIndex.first - mUpperLeftIndex.first ) * ( mLowerRightIndex.second - mUpperLeftIndex.second ) );
	std::size_t jh = 0, jv = 0;
	for( jh = mUpperLeftIndex.first; jh <= mLowerRightIndex.first; jh++ ) {
		for( jv = mUpperLeftIndex.second; jv <= mLowerRightIndex.second; jv++ ) {
			mAllCoordinates.push_back(std::make_pair( jh, jv ));
		}
	}
}

/*------------------------------------------------------------------*/


/*--------------------Constructors and destructors------------------*/

template <typename T>
Axis<T>::Axis( const Plot2D<T>& associatedPlot, const std::size_t width, const std::size_t height, const ImageCoordinate startCoordinate )
	: mPlot(associatedPlot)
{
	if( width >= height ) { //Horizontal axis
		mIsHorizontal = true;
		setAxisElement( Pixel<T,std::string>( T(), "_" ) );
	} else { //Vertical axis
		mIsHorizontal = false;
		setAxisElement( Pixel<T,std::string>( T(), "|" ) );
	}

	//Compute the coordinates in the plot where the axis will be shown
	mAxisCoordinates = std::vector<ImageCoordinate>( width*height );
	mUpperLeftIndex = startCoordinate;
	std::size_t j = 0;
	if( width > 0 && height > 0 ) {
		std::size_t jh = 0, jv = 0;
		for( jh = 0; jh < width; jh++  ) {
			for( jv = 0; jv < height; jv++ ) {
				mAxisCoordinates.at(j) = std::make_pair( startCoordinate.first + jh, startCoordinate.second + jv );
				j++;
			}
		}
		mLowerRightIndex = mAxisCoordinates.at(j-1); //The last coordinate from the loops will be the lower-rightmost of the range
	} else {
		mAxisCoordinates.at(0) = startCoordinate;
		mUpperLeftIndex = startCoordinate;
		mLowerRightIndex = startCoordinate;
	}

	//Update the vector of all coordinates, accounting for the above mAxisCoordinates
	mUpdateAllCoordinates();
}
/*------------------------------------------------------------------*/


/*---------------------Getters and Setters--------------------------*/

template <typename T>
const Pixel<T,std::string>& Axis<T>::at( const ImageCoordinate& coord ) const
{
	//Perform lookups in all coordinate vectors to find out what part of the axis is located at coord
	if( std::find(mAxisCoordinates.begin(), mAxisCoordinates.end(), coord) != mAxisCoordinates.end() ) {
		return getAxisElement();
	} else if( std::find(mLabelCoordinates.begin(), mLabelCoordinates.end(), coord) != mLabelCoordinates.end() ) {
		return mLabel.at( std::find(mLabelCoordinates.begin(), mLabelCoordinates.end(), coord) - mLabelCoordinates.begin() );
	} else if( std::find(mTicksCoordinates.begin(), mTicksCoordinates.end(), coord) != mTicksCoordinates.end() ) {
		return mTicks.at( std::find(mTicksCoordinates.begin(), mTicksCoordinates.end(), coord) - mTicksCoordinates.begin() );
	} else {
		return emptyPixel;
	}
}

template <typename T>
const std::vector<ImageCoordinate>& Axis<T>::getAxisCoordinates() const
{
	return mAxisCoordinates;
}

template <typename T>
const std::vector<ImageCoordinate>& Axis<T>::getLabelCoordinates() const
{
	return mLabelCoordinates;
}

template <typename T>
const std::vector<ImageCoordinate>& Axis<T>::getCoordinates() const
{
	return mAllCoordinates;
}

template <typename T>
const Pixel<T,std::string>& Axis<T>::getAxisElement() const
{
	return mAxisElement;
}

template <typename T>
const std::size_t Axis<T>::getLabelOffset() const
{
	return mLabelOffset;
}

template <typename T>
void Axis<T>::setAxisElement( const Pixel<T,std::string>& axisElement)
{
	mAxisElement = axisElement;
}

template <typename T>
void Axis<T>::setAxisColor( const T& color )
{
	mAxisElement.setColor(color);
}

template <typename T>
void Axis<T>::setLabelOffset( std::size_t labelOffset )
{
	mLabelOffset = labelOffset;
}

template <typename T>
void Axis<T>::setAxisSymbol( const std::string& axisSymbol )
{
	mAxisElement.setSymbol(axisSymbol);
}

/*------------------------------------------------------------------*/


/*--------------------------Modifiers-------------------------------*/

template <typename T>
void Axis<T>::addLabel( const std::string& label, float position, bool leftOrBelow , bool rotateLabel )
{
	if( mLabel.size() > 0 ) return; //A label already exists

	//Set mLabel equal to the label: "transform" the string "label" to a vector of Pixels
	mLabel = std::vector<Pixel<T,std::string>>(label.size());
	for( int j = 0; j < label.size(); j++ ) {
		mLabel.at(j) = Pixel<T,std::string>(getAxisElement().getColor(), std::string(1,label.at(j)));
	}
	std::size_t offsetFromAxis = getLabelOffset() + 1; //Offset in number of Pixels between label and axis

	// Add the label, accounting for all parameters and get back
	// the potential horizontal and vertical offsets required to accomodate the label
	auto totalOffset = mAddAttribute( mLabel, position, offsetFromAxis, leftOrBelow, mLabelCoordinates , rotateLabel );

	//Move the axis by the required offset to make space for the label
	mShift( mAxisCoordinates, totalOffset.first, totalOffset.second );
	//mShift( hTotalOffset, vTotalOffset, mTicksCoordinates )

	mUpdateAllCoordinates();
}

template <typename T>
void Axis<T>::addTicks( const std::vector<Tick>& ticks, bool leftOrBelow )
{
	float position = 0.0;
	std::vector< Pixel<T,std::string> > tickBuf;
	Pixel<T,std::string> defaultPixel(getAxisElement().getColor(), " ");

	std::pair<int,int> maxOffset = std::make_pair(std::numeric_limits<int>::min(),std::numeric_limits<int>::min());

	//Determine maximal tick length in order to insert spacings in shorter ticks
	std::size_t maxTickLength = 0;
	for( auto& tick : ticks ) {
		maxTickLength = std::max( tick.first.size(), maxTickLength );
	}

	//Loop over ticks and add them one by one
	for( auto& tick : ticks ) {

		position = tick.second;

		//Set tickBuf equal to this tick: "transform" the string "tick.first" to a vector of Pixels
		tickBuf = std::vector< Pixel<T,std::string> >(maxTickLength,defaultPixel);
		for( int j = 0; j < maxTickLength - tick.first.size(); j++ ) {
			mTicks.push_back(defaultPixel);
		}
		for( int j = 0; j < tick.first.size(); j++ ) {
			tickBuf.at(j + maxTickLength - tick.first.size()).setSymbol(std::string(1,tick.first.at(j)));
			mTicks.push_back( tickBuf.at(j + maxTickLength - tick.first.size()) );
		}

		// Add this tick, accounting for all parameters and get back
		// the potential horizontal and vertical offsets required to accomodate the label
		auto totalOffset = mAddAttribute( tickBuf, position, 0, leftOrBelow, mTicksCoordinates, false );
		if(totalOffset.first > maxOffset.first) maxOffset.first = totalOffset.first;
		if(totalOffset.second > maxOffset.second) maxOffset.second = totalOffset.second;
	}

	mShift( mAxisCoordinates, maxOffset.first, maxOffset.second );
	//mShift( mLabelCoordinates, maxOffset.first, maxOffset.second );
	mUpdateAllCoordinates();
}

/*------------------------------------------------------------------*/
