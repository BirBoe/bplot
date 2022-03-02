#include "../header/plot2d.h"

/*-------------------Private member functions-----------------------*/

template <typename T>
void Plot2D<T>::mClear()
{
	for( auto& vectorOfCoordinates : mCoordinatesOfDataPoints ) {
		for( auto& c : vectorOfCoordinates ) {
				this->at( c.first, c.second ) = emptyPixel;
			}
	}
}

template <typename T>
void Plot2D<T>::mDataSetToCoordinates( std::size_t jDataSet )
{
	//Compute DataPoints and set pixels for this DataSet in image
	std::size_t i = 0;
	for( auto& dataPoint : *(mDataSets.at(jDataSet)) ) { //Loop over all (x,y) pairs in data
		//Remove pixels for this DataSet from image
		this->at( mCoordinatesOfDataPoints.at(jDataSet).at(i).first, mCoordinatesOfDataPoints.at(jDataSet).at(i).second ) = emptyPixel;

		//Compute DataPoints and set pixels for this DataSet in image
		mCoordinatesOfDataPoints.at(jDataSet).at(i) = mDataPointToCoordinate(dataPoint);
		this->at( mCoordinatesOfDataPoints.at(jDataSet).at(i).first, mCoordinatesOfDataPoints.at(jDataSet).at(i).second ) = mPlotMarkers.at(jDataSet);
		i++;
	}
	this->at(0,0) = emptyPixel; //(0,0) is a "dump pixel" that collects data points that lie outside the frame. It always has to be cleared
}

template <typename T>
void Plot2D<T>::mAllDataSetsToCoordinates()
{
	for( std::size_t jDataSet = 0; jDataSet < mDataSets.size(); jDataSet++ ) {
		mDataSetToCoordinates(jDataSet);
	}
}

template <typename T>
ImageCoordinate Plot2D<T>::mDataPointToCoordinate( const DataPointXY& dataPoint )
{
	std::size_t xReturn = static_cast<std::size_t>( (mXScalingFunction(dataPoint.first)-mXScalingFunction(mXPlotRange.first))/(mXScalingFunction(mXPlotRange.second)+mXScalingFunction(mXPlotRange.first)) * (mDataFrameEnd.first - mDataFrameStart.first) ) + mDataFrameStart.first;
	if( xReturn > mDataFrameEnd.first ) return std::make_pair(0,0); //If the point is outside the data frame, place it outside the image

	std::size_t yReturn = mHeight - 1 - ( static_cast<std::size_t>( (mYScalingFunction(dataPoint.second)-mYScalingFunction(mYPlotRange.first))/(mYScalingFunction(mYPlotRange.second)+mYScalingFunction(mYPlotRange.first)) * (mDataFrameEnd.second - mDataFrameStart.second) ) + mDataFrameStart.second );
	if( yReturn > mDataFrameEnd.second ) return std::make_pair(0,0); //If the point is outside the data frame, place it outside the image

	return std::make_pair(xReturn,yReturn);
}

/*------------------------------------------------------------------*/


/*Constructors and destructors*/

template <typename T>
Plot2D<T>::Plot2D( std::size_t width, std::size_t height ) : Image<T,std::string>(width,height)
{
	mWidth = width;
	mHeight = height;
	this->setPixels(emptyPixel); //Set all pixels in the image to empty
  if( width > 2 && height > 2 )
  {
	  addVerticalAxis( 0, 1, height - 3, T(), "Testlabel", 0.3, true, true ); //Left
    addVerticalAxis( width - 1, 1, height - 3, T() ); //Right
    addHorizontalAxis( 0, 5, width - 2, T() ); //Top
    addHorizontalAxis( height - 3, 5, width - 2, T(), "Testlabel", 0.4, true ); //Bottom

    mDataFrameStart = std::make_pair(5,2);
  	mDataFrameEnd = std::make_pair(width-2,height-4);
		setPlotRange( std::make_pair(0.,width-3), std::make_pair(0.,height-3) );
  }
}
/*-----------------------------*/


/*-------Getters and Setters--------*/
template <typename T>
void Plot2D<T>::setXAxisScaling( const std::function<double(double)>& xScalingFunction )
{
	mXScalingFunction = xScalingFunction;
	mAllDataSetsToCoordinates();
}

template <typename T>
void Plot2D<T>::setYAxisScaling( const std::function<double(double)>& yScalingFunction )
{
	mYScalingFunction = yScalingFunction;
	mAllDataSetsToCoordinates();
}

template <typename T>
void Plot2D<T>::setPlotRange( const std::pair<double,double>& xPlotRange, const std::pair<double,double>& yPlotRange )
{
	mXPlotRange = xPlotRange;
	mYPlotRange = yPlotRange;
	mAllDataSetsToCoordinates();
}

/*----------------------------------*/
template <typename T>
void Plot2D<T>::addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color )
{
  if( vPosStart > vPosEnd ) std::swap(vPosStart,vPosEnd);
	mVerticalAxes.push_back( Axis<T>( *this, 1, vPosEnd - vPosStart + 1, std::make_pair( hPos, vPosStart ) ) );
	mVerticalAxes.back().setAxisColor( color );
	for( auto& idx : mVerticalAxes.back().getCoordinates() ) {
		this->at( idx.first, idx.second ) = mVerticalAxes.back().at(idx);
	}
}

template <typename T>
void Plot2D<T>::addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color )
{
	if( hPosStart > hPosEnd ) std::swap(hPosStart,hPosEnd);
	mHorizontalAxes.push_back( Axis<T>( *this, hPosEnd - hPosStart + 1, 1, std::make_pair( hPosStart, vPos ) ) );
	mHorizontalAxes.back().setAxisColor( color );
	for( auto& idx : mHorizontalAxes.back().getCoordinates() ) {
		this->at( idx.first, idx.second ) = mHorizontalAxes.back().at(idx);
	}
}

template <typename T>
void Plot2D<T>::addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color, const std::string& label, float relativeLabelPosition, bool labelLeft, bool rotateLabel )
{
  if( vPosStart > vPosEnd ) std::swap(vPosStart,vPosEnd);
	mVerticalAxes.push_back( Axis<T>( *this, 1, vPosEnd - vPosStart + 1, std::make_pair( hPos, vPosStart ) ) );
	mVerticalAxes.back().setAxisColor( color );
	mVerticalAxes.back().addLabel(label, relativeLabelPosition, labelLeft, rotateLabel);

	/*TEST*/
	std::vector< std::pair<std::string,float> > testTicks;
	testTicks.reserve( 11 );
	for( int t = 0; t < 11; t++ ) {
		testTicks.push_back( std::make_pair( std::to_string(10*t), 1. - 0.1*t ) );
	}
	mVerticalAxes.back().addTicks(testTicks,false);
	/*----*/

	for( auto& idx : mVerticalAxes.back().getCoordinates() ) {
		this->at( idx.first, idx.second ) = mVerticalAxes.back().at(idx);
	}
}

template <typename T>
void Plot2D<T>::addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color, const std::string& label, float relativeLabelPosition, bool labelBelow )
{
	if( hPosStart > hPosEnd ) std::swap(hPosStart,hPosEnd);
	mHorizontalAxes.push_back( Axis<T>( *this, hPosEnd - hPosStart + 1, 1, std::make_pair( hPosStart, vPos ) ) );
	mHorizontalAxes.back().setAxisColor( color );
	mHorizontalAxes.back().addLabel(label, relativeLabelPosition, labelBelow);

	/*TEST*/
	std::vector< std::pair<std::string,float> > testTicks;
	testTicks.reserve( 10 );
	for( int t = 0; t < 10; t++ ) {
		testTicks.push_back( std::make_pair( std::to_string(t), 0.1*t ) );
	}
	mHorizontalAxes.back().addTicks(testTicks, true);
	/*----*/

	for( auto& idx : mHorizontalAxes.back().getCoordinates() ) {
		this->at( idx.first, idx.second ) = mHorizontalAxes.back().at(idx);
	}
}

template <typename T>
void Plot2D<T>::addText( std::size_t hPos, std::size_t vPos, const std::string& text ) //Adds (horizontal) text starting at position (hPos,vPos)
{
  this->at( hPos, vPos ).setSymbol(text);
  for( int j = 1; j < text.length(); j++ ) {
    this->at( hPos+j, vPos ).setSymbol("");
  }
  mText.push_back(std::make_pair(text,std::make_pair(hPos,vPos)));
}

template <typename T>
void Plot2D<T>::addDataSet( std::shared_ptr<const DataSet> dataSet, Pixel<T,std::string> plotMarker )
{
	//If this is the first data set added to the plot (mData empty):
	//Set the plot range to fit all data points, i.e. extract min and max x and y values from data
	if( mDataSets.size() == 0 ) {
		double xmin = 0., xmax = 0., ymin = 0., ymax = 0.;
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
			//Set the plot range
			setPlotRange( std::make_pair( xmin - dx, xmax + dx ), std::make_pair( ymin - dy, ymax + dy ) );
			//TODO: Add ticks

	}

	mDataSets.push_back(dataSet);
  mPlotMarkers.push_back(plotMarker);

  mCoordinatesOfDataPoints.push_back(std::vector<ImageCoordinate>(dataSet->size()));
	mDataSetToCoordinates( mCoordinatesOfDataPoints.size() - 1 );
}
