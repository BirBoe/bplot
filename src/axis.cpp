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

	for( std::size_t jh = 0; jh < hRange; jh++) {
		for( std::size_t jv = 0; jv < vRange; jv++ ) {
			coordinateVector.at(firstIndex + jh + jv * hRange) = std::make_pair( firstNewCoordinate.first + jh, firstNewCoordinate.second + jv );
		}
	}
}

template <typename T>
std::pair<int,int> Axis<T>::mAddAttribute( const std::vector< Pixel<T,std::string> >& attribute, float position, std::size_t offsetFromAxis, bool leftOrBottom, std::vector<ImageCoordinate>& coordinateVector , bool rotate )
{
	if( position > 1. || position < 0. ) position = 0.;

	int hTotalOffset = 0, vTotalOffset = 0;
	ImageCoordinate startIndex = mAxisCoordinates.at(0);

	int overallShift = 0; //If parts of the axis, labels, etc. go out of the index range of the plot, they will be shifted back to within the range

	if( mIsHorizontal ) {
		std::size_t hOffset = static_cast<std::size_t>( position * (mLowerRightIndex.first - mUpperLeftIndex.first + 1) );

		if( !leftOrBottom ) { //Label is above the axis

			//Check if axis and attribute will fit into the plot. If not: set overallShift accordingly
			if( startIndex.second  < offsetFromAxis ) {
				overallShift = offsetFromAxis - startIndex.second;
			}

			//Attribute placement
			mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startIndex.first + hOffset, startIndex.second - offsetFromAxis + overallShift ), attribute.size(), 1 );

			//The boundaries of the axis+attributes need to be changed:
			if( mUpperLeftIndex.second + offsetFromAxis > startIndex.second ) {
				mUpperLeftIndex.second = startIndex.second - offsetFromAxis;
			}

		} else { //Label is below the axis

			//Check if axis and attribute will fit into the plot. If not: set overallShift accordingly
			if( startIndex.second + offsetFromAxis >= mPlot.getHeight() ) {
				overallShift = mPlot.getHeight() - 1 - startIndex.second - offsetFromAxis;
			}

			//Attribute placement
			mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startIndex.first + hOffset, startIndex.second + offsetFromAxis + overallShift ), attribute.size(), 1 );

			//The boundaries of the axis+attributes need to be changed:
			if( mLowerRightIndex.second < startIndex.second + offsetFromAxis ) {
				mLowerRightIndex.second = startIndex.second + offsetFromAxis;
			}

		}

		//Account for the overall vertical Shift of the axis
		vTotalOffset += overallShift;
		mUpperLeftIndex.second += overallShift;
		mLowerRightIndex.second += overallShift;

	} else {
		std::size_t vOffset = static_cast<std::size_t>( position * ( mLowerRightIndex.second - mUpperLeftIndex.second + 1 ) );

		if( leftOrBottom ) { //Label is left of the axis

			//Check if axis and attribute will fit into the plot. If not: set overallShift accordingly
			if( rotate && startIndex.first < offsetFromAxis ) {
				overallShift = offsetFromAxis - startIndex.first;
			}
			if( !rotate && startIndex.first < offsetFromAxis + attribute.size() ) {
				overallShift = offsetFromAxis + attribute.size() - startIndex.first;
			}

			//Attribute placement
			if(rotate) {
				mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startIndex.first - offsetFromAxis + overallShift, startIndex.second + vOffset ), 1, attribute.size() );
			} else {
				mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startIndex.first - offsetFromAxis + overallShift, startIndex.second + vOffset ), attribute.size(), 1 );
			}

			//The axis will be potentially moved to the right by hTotalOffset
			if( !rotate ) {
				hTotalOffset += attribute.size();
				offsetFromAxis += attribute.size();
			}

			//The boundaries of the axis+attributes need to be changed:
			if( mUpperLeftIndex.first + offsetFromAxis > startIndex.first ) {
				mUpperLeftIndex.first = startIndex.first - offsetFromAxis;
			}

		} else { //Attribute is right of the axis

			//Check if axis and attribute will fit into the plot. If not: set overallShift accordingly
			if( rotate && startIndex.first + offsetFromAxis >= mPlot.getWidth() ) {
				overallShift = mPlot.getWidth() - 1 - startIndex.first - offsetFromAxis;
			}
			if( !rotate && startIndex.first + offsetFromAxis + attribute.size() >= mPlot.getWidth() ) {
				overallShift = mPlot.getWidth() - 1 - startIndex.first - offsetFromAxis - attribute.size();
			}

			//Attribute placement
			if(rotate) {
				mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startIndex.first + offsetFromAxis + overallShift, startIndex.second + vOffset ), 1, attribute.size() );
			} else {
				mAddRangeToCoordinateVector( coordinateVector, std::make_pair( startIndex.first + offsetFromAxis + overallShift, startIndex.second + vOffset ), attribute.size(), 1 );
			}

			if( !rotate ) {
				offsetFromAxis += attribute.size();
			}

			//The boundaries of the axis+attributes need to be changed:
			if( mLowerRightIndex.first < startIndex.first + offsetFromAxis ) {
				mLowerRightIndex.first = startIndex.first + offsetFromAxis;
			}

		}

		//Account for the overall horizontal Shift of the axis
		hTotalOffset += overallShift;
		mUpperLeftIndex.first += overallShift;
		mLowerRightIndex.first += overallShift;

	}

	return std::make_pair(hTotalOffset,vTotalOffset);
}


template <typename T>
void Axis<T>::mShift( int horizontalShift, int verticalShift, std::vector<ImageCoordinate>& indexVector )
{
	//TODO: What happens if size_t becomes negative somewhere below?
	for( auto& idx : indexVector ) {
		idx.first += horizontalShift;
		idx.second += verticalShift;
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


/*Constructors and destructors*/

template <typename T>
Axis<T>::Axis( const Plot2D<T>& associatedPlot, const std::size_t width, const std::size_t height, const ImageCoordinate startIndex )
	: mPlot(associatedPlot)
{
	if( width >= height ) { //Horizontal axis
		mIsHorizontal = true;
		setAxisElement( Pixel<T,std::string>( T(), "_" ) );
	} else { //Vertical axis
		mIsHorizontal = false;
		setAxisElement( Pixel<T,std::string>( T(), "|" ) );
	}

	//Set indices representing the axis and the index range as given by mUpperLeftIndex and mLowerRightIndex
	mAxisCoordinates = std::vector<ImageCoordinate>( width*height );
	mUpperLeftIndex = startIndex;
	std::size_t j = 0;
	if( width > 0 && height > 0 ) {
		std::size_t jh = 0, jv = 0;
		for( jh = 0; jh < width; jh++  ) {
			for( jv = 0; jv < height; jv++ ) {
				mAxisCoordinates.at(j) = std::make_pair( startIndex.first + jh, startIndex.second + jv );
				j++;
			}
		}
		mLowerRightIndex = mAxisCoordinates.at(j-1);
	} else {
		mAxisCoordinates.at(0) = startIndex;
		mUpperLeftIndex = startIndex;
		mLowerRightIndex = startIndex;
	}

	mUpdateAllCoordinates();
}
/*-----------------------------*/


/*-------Getters and Setters--------*/
template <typename T>
const Pixel<T,std::string>& Axis<T>::at( const ImageCoordinate& idx ) const
{
	if( std::find(mAxisCoordinates.begin(), mAxisCoordinates.end(), idx) != mAxisCoordinates.end() ) {
		return getAxisElement();
	} else if( std::find(mLabelCoordinates.begin(), mLabelCoordinates.end(), idx) != mLabelCoordinates.end() ) {
		return mLabel.at( std::find(mLabelCoordinates.begin(), mLabelCoordinates.end(), idx) - mLabelCoordinates.begin() );
	} else if( std::find(mTicksCoordinates.begin(), mTicksCoordinates.end(), idx) != mTicksCoordinates.end() ) {
		return mTicks.at( std::find(mTicksCoordinates.begin(), mTicksCoordinates.end(), idx) - mTicksCoordinates.begin() );
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

/*----------------------------------*/

/*-------------Modifiers------------*/
template <typename T>
void Axis<T>::addLabel( const std::string& label, float position, bool leftOrBottom , bool rotateLabel )
{
	if( mLabel.size() > 0 ) return; //A label already exists
	mLabel = std::vector<Pixel<T,std::string>>(label.size());
	for( int j = 0; j < label.size(); j++ ) {
		mLabel.at(j) = Pixel<T,std::string>(getAxisElement().getColor(), std::string(1,label.at(j)));
	}
	std::size_t offsetFromAxis = getLabelOffset() + 1; //Offset in coordinates between label and axis

	// Add the label, accounting for all parameters and get back
	// the coordinate offset totalOffset = (horizontalOffset,verticalOffset) required to accomodate the label
	auto totalOffset = mAddAttribute( mLabel, position, offsetFromAxis, leftOrBottom, mLabelCoordinates , rotateLabel );

	//Move the axis by totalOffset to make space for the label
	mShift( totalOffset.first, totalOffset.second, mAxisCoordinates );
	//mShift( hTotalOffset, vTotalOffset, mTicksCoordinates )

	mUpdateAllCoordinates();
}

template <typename T>
void Axis<T>::addTicks( const std::vector<Tick>& ticks, bool leftOrBottom )
{
	float position = 0.0;
	int hTotalOffset = 0, vTotalOffset = 0;
	std::vector< Pixel<T,std::string> > tickBuf;

	bool first = true;
	for( auto& tick : ticks ) {

		position = tick.second;
		tickBuf.resize( tick.first.size() );
		for( int j = 0; j < tick.first.size(); j++ ) {
			tickBuf.at(j) = Pixel<T,std::string>(getAxisElement().getColor(), std::string(1,tick.first.at(j)));
			mTicks.push_back( tickBuf.at(j) );
		}
		auto totalOffset = mAddAttribute( tickBuf, position, 1, leftOrBottom, mTicksCoordinates , false );
		/*hTotalOffset = std::max( totalOffset.first, hTotalOffset );
		vTotalOffset = std::max( totalOffset.second, vTotalOffset );
		mShift( hTotalOffset, vTotalOffset, mAxisCoordinates );*/
		if( first ) {
			mShift( totalOffset.first, totalOffset.second, mAxisCoordinates );
			mShift( totalOffset.first, totalOffset.second, mLabelCoordinates );
		}
		first = false;
	}

	mUpdateAllCoordinates();
}


/*----------------------------------*/
