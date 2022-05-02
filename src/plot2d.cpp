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
void Plot2D<T>::mClear()
{
	for( auto& vectorOfCoordinates : mCoordinatesOfDataPoints ) {
		for( auto& coord : vectorOfCoordinates ) {
				this->at(coord) = emptyPixel;
			}
	}
}

template <typename T>
void Plot2D<T>::mDataSetToCoordinates( std::size_t jDataSet )
{
	//Compute ImageCoordinates and set pixels for the DataSet mDataSets.at(jDataSet) in the image
	std::size_t i = 0;
	for( auto& dataPoint : *(mDataSets.at(jDataSet)) ) { //Loop over all DataPoints [(x,y) pairs] in the data set
		//Remove pixels for this DataSet from image
		this->at( mCoordinatesOfDataPoints.at(jDataSet).at(i) ) = emptyPixel;

		//Compute ImageCoordinates and set the pixels for this data set at the (new) ImageCoordinates in the Image
		mCoordinatesOfDataPoints.at(jDataSet).at(i) = mDataPointToCoordinate(dataPoint);
		this->at( mCoordinatesOfDataPoints.at(jDataSet).at(i) ) = mPlotMarkers.at(jDataSet);
		i++;
	}
	/* (0,0) is a "dump pixel" that collects data points that would lie outside the data frame.
	 * It could have be returned by mDataPointToCoordinate(), so we have to "clean it up" here. */
	this->at( std::make_pair(0,0) ) = emptyPixel;
}

template <typename T>
void Plot2D<T>::mAllDataSetsToCoordinates()
{
	for( std::size_t jDataSet = 0; jDataSet < mDataSets.size(); jDataSet++ ) {
		mDataSetToCoordinates(jDataSet);
	}
}

template <typename T>
void Plot2D<T>::mUpdateAxisCoordinates()
{
	for( auto& axis : mVerticalAxes ) {
		for( auto& coord : axis.getCoordinates() ) {
			this->at(coord) = axis.at(coord); //Get the coordinates of the axis and add the correct Pixels to the plot
		}
	}
	for( auto& axis : mHorizontalAxes ) {
		for( auto& coord : axis.getCoordinates() ) {
			this->at(coord) = axis.at(coord);
		}
	}
}

template <typename T>
ImageCoordinate Plot2D<T>::mDataPointToCoordinate( const DataPointXY& dataPoint ) const
{
	/*Transform the x value of the dataPoint to a horizontal coordinate along the x axis.
	 * Account for the scaling function and the plot range. */
	float scaling_fac = (mXScalingFunction(dataPoint.first)-mXScalingFunction(mXPlotRange.first))/(mXScalingFunction(mXPlotRange.second)-mXScalingFunction(mXPlotRange.first));
	std::size_t xReturn = static_cast<std::size_t>( scaling_fac * (mDataFrameEnd.first - mDataFrameStart.first) ) + mDataFrameStart.first;
	if( xReturn > mDataFrameEnd.first ) return std::make_pair(0,0); //(0,0) is a "dump pixel" that collects data points that would lie outside the data frame.

	//Same for the y value
	scaling_fac = (mYScalingFunction(dataPoint.second)-mYScalingFunction(mYPlotRange.first))/(mYScalingFunction(mYPlotRange.second)-mYScalingFunction(mYPlotRange.first));
	std::size_t yReturn = this->getHeight() - 1 - ( static_cast<std::size_t>( scaling_fac * (mDataFrameEnd.second - mDataFrameStart.second) ) + mDataFrameStart.second );
	if( yReturn > mDataFrameEnd.second ) return std::make_pair(0,0); //(0,0) is a "dump pixel" that collects data points that would lie outside the data frame.

	return std::make_pair(xReturn,yReturn);
}

/*------------------------------------------------------------------*/


/*-------------------Constructors and destructors-------------------*/

template <typename T>
Plot2D<T>::Plot2D( std::size_t width, std::size_t height ) : Image<T,std::string>(width,height)
{
	this->setAllPixels(emptyPixel); //Set all pixels in the image to empty
  if( width > 2 && height > 2 )
  {
		//Add default axes
	  addVerticalAxis( 0, 1, height - 3, T(), "y", 0.3, true, true ); //Left
    addVerticalAxis( width - 1, 1, height - 3, T() ); //Right
    addHorizontalAxis( height - 3, 5, width - 2, T(), "x", 0.4, true ); //Bottom
    addHorizontalAxis( 0, 5, width - 2, T() ); //Top

		//Set the coordinate ranges in the plot and a default plot range
    mDataFrameStart = std::make_pair(5,2);
  	mDataFrameEnd = std::make_pair(width-2,height-4);
		setPlotRange( std::make_pair(0.,width-3), std::make_pair(0.,height-3) );
  }
}

template <typename T>
Plot2D<T>::Plot2D( std::size_t width, std::size_t height, const std::string& xAxisLabel, const std::string& yAxisLabel ) : Image<T,std::string>(width,height)
{
	this->setAllPixels(emptyPixel); //Set all pixels in the image to empty
  if( width > 2 && height > 2 )
  {
		//Add default axes
	  addVerticalAxis( 0, 1, height - 3, T(), yAxisLabel, 0.3, true, true ); //Left
    addVerticalAxis( width - 1, 1, height - 3, T() ); //Right
    addHorizontalAxis( height - 3, 5, width - 2, T(), xAxisLabel, 0.4, true ); //Bottom
    addHorizontalAxis( 0, 5, width - 2, T() ); //Top

		//Set the coordinate ranges in the plot and a default plot range
    mDataFrameStart = std::make_pair(5,2);
  	mDataFrameEnd = std::make_pair(width-2,height-4);
		setPlotRange( std::make_pair(0.,width-3), std::make_pair(0.,height-3) );
  }
}

/*------------------------------------------------------------------*/


/*-----------------------Getters and Setters------------------------*/

template <typename T>
void Plot2D<T>::setXAxisScaling( const std::function<double(double)>& xScalingFunction )
{
	mXScalingFunction = xScalingFunction;
	mAllDataSetsToCoordinates(); //The new scaling changes the position of data points in the data frame.
}

template <typename T>
void Plot2D<T>::setYAxisScaling( const std::function<double(double)>& yScalingFunction )
{
	mYScalingFunction = yScalingFunction;
	mAllDataSetsToCoordinates(); //The new scaling changes the position of data points in the data frame.
}

template <typename T>
void Plot2D<T>::setPlotRange( const std::pair<double,double>& xPlotRange, const std::pair<double,double>& yPlotRange )
{
	mXPlotRange = xPlotRange;
	mYPlotRange = yPlotRange;
	mAllDataSetsToCoordinates(); //The new plot range changes the position of data points in the data frame.
}

/*------------------------------------------------------------------*/


/*----------------------------Modifiers-----------------------------*/

template <typename T>
void Plot2D<T>::addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color )
{
  if( vPosStart > vPosEnd ) std::swap(vPosStart,vPosEnd);
	mVerticalAxes.push_back( Axis<T>( *this, 1, vPosEnd - vPosStart + 1, std::make_pair( hPos, vPosStart ) ) );
	mVerticalAxes.back().setAxisColor( color );

	//Update the coordinates to account for the new axis
	mUpdateAxisCoordinates();
}

template <typename T>
void Plot2D<T>::addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color )
{
	if( hPosStart > hPosEnd ) std::swap(hPosStart,hPosEnd);
	mHorizontalAxes.push_back( Axis<T>( *this, hPosEnd - hPosStart + 1, 1, std::make_pair( hPosStart, vPos ) ) );
	mHorizontalAxes.back().setAxisColor( color );

	//Get the coordinates of the new axis and add the correct Pixels to the plot
	mUpdateAxisCoordinates();
}

template <typename T>
void Plot2D<T>::addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color, const std::string& label, float relativeLabelPosition, bool labelLeft, bool rotateLabel )
{
  if( vPosStart > vPosEnd ) std::swap(vPosStart,vPosEnd);
	mVerticalAxes.push_back( Axis<T>( *this, 1, vPosEnd - vPosStart + 1, std::make_pair( hPos, vPosStart ) ) );
	mVerticalAxes.back().setAxisColor( color );
	mVerticalAxes.back().addLabel(label, relativeLabelPosition, labelLeft, rotateLabel);

	//Update the coordinates to account for the new axis
	mUpdateAxisCoordinates();
}

template <typename T>
void Plot2D<T>::addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color, const std::string& label, float relativeLabelPosition, bool labelBelow )
{
	if( hPosStart > hPosEnd ) std::swap(hPosStart,hPosEnd);
	mHorizontalAxes.push_back( Axis<T>( *this, hPosEnd - hPosStart + 1, 1, std::make_pair( hPosStart, vPos ) ) );
	mHorizontalAxes.back().setAxisColor( color );
	mHorizontalAxes.back().addLabel(label, relativeLabelPosition, labelBelow);

	//Update the coordinates to account for the new axis
	mUpdateAxisCoordinates();
}

template <typename T>
void Plot2D<T>::addText( const std::string& text, ImageCoordinate& textPosition )
{
	//Set the text as a single Pixel and set the following Pixels to "" to "make space"
  this->at(textPosition).setSymbol(text);
  for( int j = 1; j < text.length(); j++ ) {
    this->at(textPosition).setSymbol("");
  }
	//Add to the text vector to keep track
  mText.push_back(std::make_pair(text,std::make_pair(textPosition.first,textPosition.second)));
}

template <typename T>
void Plot2D<T>::addDataSet( std::shared_ptr<const DataSet> dataSet, Pixel<T,std::string> plotMarker )
{
	//If this is the first data set added to the plot (mData empty):
	//Set the plot range to fit all data points, i.e. extract min and max x and y values from data
	if( mDataSets.size() == 0 ) {
		double xmin = std::numeric_limits<double>::max(), xmax = std::numeric_limits<double>::min(), ymin = xmin, ymax = xmax;
			for( auto& dataPoint : *dataSet ) {
				if( dataPoint.first < xmin ) {
					xmin = dataPoint.first;
				} else if( dataPoint.first > xmax ) {
					xmax = dataPoint.first;
				}
				if( dataPoint.second < ymin ) {
					ymin = dataPoint.second;
				} else if( dataPoint.second > ymax ) {
					ymax = dataPoint.second;
				}
			}

			//Add an offset (otherwise some data points lie on the axes and will not be shown)
			double dx = (xmax-xmin)/( dataSet->size() - 1 );
			double dy = (ymax-ymin)/( dataSet->size() - 1 );
			dx = (xmax-xmin)/20;
			dy = (ymax-ymin)/10;

			//Set the plot range
			setPlotRange( std::make_pair( xmin - dx, xmax + dx ), std::make_pair( ymin - dy, ymax + dy ) );

			//Compute equidistant ticks and add to the axes (if any)
			unsigned int noOfTicks = 10;
			std::size_t jTick = 0;
			float tickstep = 1./noOfTicks;
			std::vector<Tick> ticks(noOfTicks);

			//Helper variables to format the axis ticks
			int tickDecimals = 1; //Number of digits in the labels
			int fac = pow(10,tickDecimals);
			std::stringstream tickFormatStream;

			if( mHorizontalAxes.size() > 0 ) {

				tickFormatStream << std::fixed << std::setprecision(tickDecimals) << 1./fac * static_cast<int>(xmin*fac);
				ticks.at(0) = std::make_pair<std::string,float>( tickFormatStream.str(), dx/(xmax-xmin+2.*dx) );
				for( jTick = 1; jTick < noOfTicks; jTick++ ) {
					tickFormatStream.str("");
					tickFormatStream << std::fixed << std::setprecision(tickDecimals) << 1./fac * static_cast<int>( ( xmin + jTick * tickstep * (xmax-xmin+2.*dx) ) * fac );
					ticks.at(jTick).first = tickFormatStream.str();
					ticks.at(jTick).second = ticks.at(jTick-1).second + tickstep;
				}
				mHorizontalAxes.at(0).addTicks(ticks,true);

			} if( mVerticalAxes.size() > 0 ) {
				tickFormatStream.str("");
				tickFormatStream << std::fixed << std::setprecision(tickDecimals) << 1./fac * static_cast<int>(ymin*fac);
				ticks.at(0) = std::make_pair<std::string,float>( tickFormatStream.str(), 1.-dy/(ymax-ymin+2.*dy) );

				for( jTick = 1; jTick < noOfTicks; jTick++ ) {
					tickFormatStream.str("");
					tickFormatStream << std::fixed << std::setprecision(tickDecimals) << 1./fac * static_cast<int>( ( ymin + jTick * tickstep * (ymax-ymin+2.*dy) ) * fac );
					ticks.at(jTick).first = tickFormatStream.str();
					ticks.at(jTick).second = ticks.at(jTick-1).second - tickstep;
				}
				mVerticalAxes.at(0).addTicks(ticks,true);

			}
			//Update the coordinates to account for the changes of the axes
			mUpdateAxisCoordinates();
	}

	//Add to the necessary data members to keep track
	mDataSets.push_back(dataSet);
  mPlotMarkers.push_back(plotMarker);
	//Compute the ImageCoordinates where data points of this data set are shown in the plot
  mCoordinatesOfDataPoints.push_back(std::vector<ImageCoordinate>(dataSet->size()));
	mDataSetToCoordinates( mCoordinatesOfDataPoints.size() - 1 );
}

/*------------------------------------------------------------------*/
