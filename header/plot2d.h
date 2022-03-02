#pragma once

/* Plot2D is a special Image (i.e. derived from class Image) representing a plot of discrete two-dimensional data. It provides:
	- Axes
	- Labels
	-
	-

*/

#include <algorithm> //std::swap
#include <memory> //Smart pointers
#include <string>
#include <utility> //std::pair

#include "../header/axis.h"
#include "../header/color_models.h"
#include "../header/image.h"
#include "../header/pixel.h"

using ImageCoordinate = std::pair<std::size_t,std::size_t>; //Pair (jh,jv) of horizontal and vertical indices in the base image
using DataPointXY = std::pair<double,double>; //Pair (x,y) of data values plotted in the image
using DataSet = std::vector<DataPointXY>; //DataSet corresponding to one "curve" in the plot

template <typename T = Mono>
class Plot2D : public Image<T,std::string>
{
private:
	inline static const Pixel<T,std::string> emptyPixel = Pixel<T,std::string>(T()," ");

	std::size_t mWidth;
	std::size_t mHeight;

	std::vector<Axis<T>> mHorizontalAxes;
	std::vector<Axis<T>> mVerticalAxes;

	std::vector<std::pair<std::string,ImageCoordinate>> mText; //Stores text in the image together with the index pair (jh,jv) of its position

	/*Index ranges for the part of the image showing the data (i.e. within the frame)*/
	ImageCoordinate mDataFrameStart; //Index pair (jh,jv) of top left corner
	ImageCoordinate mDataFrameEnd; //Index pair (jh,jv) of lower right corner
	std::pair<double,double> mXPlotRange; //Plot range on the x axis
	std::pair<double,double> mYPlotRange; //Plot range on the y axis

	//Functions defining the scaling of axes (i.e. transformation from original values to relative position on axes)
	std::function<double(double)> mXScalingFunction = [](double x){ return x; }; //Default: linear
	std::function<double(double)> mYScalingFunction = [](double y){ return y; }; //Default: linear

	//Vector of pointers to vectors holding the data shown in the plot for every data (i.e. for every curve).
	//Every data set consists of a vector of pairs of doubles (x and y values)
	std::vector< std::shared_ptr<const DataSet> > mDataSets;

	//Vector of ImageCoordinates in the base image for Pixels of every data set shown in the same order as mDataSets (i.e. mDataSets.at(j) corresponds to mImageCoordinatesOfDataPoints.at(j))
	std::vector< std::vector<ImageCoordinate> > mCoordinatesOfDataPoints;

	//Markers for every curve in the same order as mData (i.e. mPlotMarkers.at(j) belongs to mData.at(j) and mDataPixels.at(j))
	std::vector< Pixel<T,std::string> > mPlotMarkers;

	void mClear(); //Clears the data frame, i.e. sets all pixels representing data points to emptyPixel
	void mDataSetToCoordinates( std::size_t jDataSet ); //Set mDataPoints.at(jData) for the data in mData.at(jData), removes the previous pixels (if any) in the image and sets the new ones.
	void mAllDataSetsToCoordinates(); //Set mDataPoints all data sets in mData.at(jData), removes the previous pixels (if any) in the image and sets the new ones.
	ImageCoordinate mDataPointToCoordinate( const DataPointXY& dataPoint ); //Transforms a pair of original data values to a point (index pair) in the image, taking into account the scaling functions and data frame boundaries

public:
	/*Constructors and destructors*/
	//Plot2D() = default; //Constructs an empty plot with all values set to zero or equivalent
	Plot2D( std::size_t width, std::size_t height );

	/*---Getters and Setters----*/
	void setXAxisScaling( const std::function<double(double)>& xScalingFunction );
	void setYAxisScaling( const std::function<double(double)>& yScalingFunction );
	void setPlotRange( const std::pair<double,double>& xPlotRange, const std::pair<double,double>& yPlotRange );
	/*--------------------------*/

	/*---------Modifiers--------*/
	/** Adds a vertical axis to the plot that extends from (hPos,vPosStart) to (hPos,vPosEnd)
		* @param hPos Horizontal coordinate of the axis
		* @param vPosStart Vertical coordinate of the start point of the axis
		* @param vPosEnd Vertical coordinate of the end of the axis
		* @param color Color of the axis
		*/
	void addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color );

	/** Adds a horizontal axis to the plot that extends from (hPosStart,vPos) to (hPosEnd,vPos).
		* @param vPos vertical coordinate of the axis
		* @param hPosStart Horizontal coordinate of the start point of the axis
		* @param hPosEnd Horizontal coordinate of the end of the axis
		* @param color Color of the axis
		*/
	void addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color );

	/** Adds a labelled vertical axis to the plot that extends from (hPos,vPosStart) to (hPos,vPosEnd). The position of the axis will be changed to make space for the label.
		* @param label Label that is added to the axis
		* @param relativeLabelPosition Position of the first character of label along the axis; has to be between 0.0 and 1.0 (otherwise set to 0.0)
		* @param labelLeft Placement relative to the axis (true: left, false: right)
		* @param rotateLabel If true: rotate the label by 90 degrees clockwise. This can be useful to save space in the plot.
		* @see addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color )
		*/
	void addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color, const std::string& label, float relativeLabelPosition = 0.5, bool labelLeft = true, bool rotateLabel = false ); //Adds a vertical axis with label

	/** Adds a labelled horizontal axis to the plot that extends from (hPosStart,vPos) to (hPosEnd,vPos). The position of the axis will be changed to make space for the label.
		* @param label Label that is added to the axis
		* @param relativeLabelPosition Position of the first character of label along the axis; has to be between 0.0 and 1.0 (otherwise set to 0.0)
		* @param labelBelow Placement relative to the axis (true: below, false: above)
		* @see addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color )
		*/
	void addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color, const std::string& label, float relativeLabelPosition = 0.5, bool labelBelow = true ); //Adds a horizontal axis with label


	void addText( std::size_t hPos, std::size_t vPos, const std::string& text ); //Adds (horizontal) text starting at position (hPos,vPos)


	void addDataSet( std::shared_ptr<const DataSet> dataSet, Pixel<T,std::string> plotMarker );

	//"REMOVERS"
	//void flush(); //Removes all data and data points

	/*--------------------------*/

	/*----------Operators----------*/

	/*-----------------------------*/
};

/*Include method definitions (needed here for the compiler because it is a class template!)*/
#include "../src/plot2d.cpp"
