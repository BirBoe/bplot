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

/**
	* The Plot2D class represents a plot of two-dimensional data that can be printed to the console (or another output stream).
	* Accordingly, it is a special type of Image and derived from the Image class.
	* A Plot2D consists of axes of a certain value range that can be scaled (e.g. linearly, logarithmically, etc.),
	* a region of the Image where data is shown (data frame), and Pixels representing the actual data.
	* Any number of data sets can be part of the same plot. Each data set can be represented in the plot
	* by another plot marker (different symbols, different colors).
	* The class has manages the data itself in its raw form (data set) and the
	* ImageCoordinates corresponding to the positions in the data frame where points from the data set are shown.
	* @tparam T Color model used for the plot (default: Mono).
	*/

template <typename T>
class Plot2D : public Image<T,std::string>
{
private:
	inline static const Pixel<T,std::string> emptyPixel = Pixel<T,std::string>(T()," ");

	std::vector<Axis<T>> mHorizontalAxes; /*!< Horizontal axes that are part of the plot. */
	std::vector<Axis<T>> mVerticalAxes; /*!< Vertical axes that are part of the plot. */

	/**
		* Text shown in the plot: strings together with coordinates of their first character in the base image.
		*/
	std::vector<std::pair<std::string,ImageCoordinate>> mText;

	/**
		* ImageCoordinate of the top left corner of the data frame, i.e. the area of the base image that shows data values.
		*/
	ImageCoordinate mDataFrameStart;

	/**
		* ImageCoordinate of the lower right corner of the data frame, i.e. the area of the base image that shows data values.
		*/
	ImageCoordinate mDataFrameEnd;

	std::pair<double,double> mXPlotRange; /*!< Range (xmin,xmax) of values represented on the x axis. */
	std::pair<double,double> mYPlotRange; /*!< Range (ymin,ymax) of values represented on the y axis. */

	/**
		* Function that holds the scaling of values along the x axis: x' = mXScalingFunction( xLinear )
		*/
	std::function<double(double)> mXScalingFunction = [](double x){ return x; };

	/**
		* Function that holds the scaling of values along the y axis: y' = mYScalingFunction( yLinear )
		*/
	std::function<double(double)> mYScalingFunction = [](double y){ return y; };

	/**
		* Data sets that are shown in the plot. Each data set is given by a pointer to a vector of DataPointXY (i.e. pairs of doubles (x and y values))
		*/
	std::vector< std::shared_ptr<const DataSet> > mDataSets;

	/**
		* Coordinates in the base image representing all data points in all data sets (one vector for each data set,
		* i.e. i.e. mCoordinatesOfDataPoints.at(j) belongs to mDataSets.at(j)).
		*/
	std::vector< std::vector<ImageCoordinate> > mCoordinatesOfDataPoints;

	/**
		* Markers (single Pixels) that represent points from the data sets in the plot (one for each data set,
		* i.e. mPlotMarkers.at(j) belongs to mDataSets.at(j))
		*/
	std::vector< Pixel<T,std::string> > mPlotMarkers;

	/**
		* Sets all Pixels representing data points in the plot to emptyPixel. This does not remove dataSets from mDataSets.
		*/
	void mClear();

	/**
		* For a single data set: Remove the data set from the plot (if already shown),
		* (re-)compute positions of Pixels in the plot and add them to mCoordinatesOfDataPoints.
		* @param jDataSet index of the data set in mDataSets
		*/
	void mDataSetToCoordinates( std::size_t jDataSet );

	/**
		* Remove all data sets from the plot (if already shown),
		* (re-)compute positions of Pixels in the plot and add them to mCoordinatesOfDataPoints.
		*/
	void mAllDataSetsToCoordinates();

	/**
		* Transform a data point to a coordinate in the base image of the plot
		* @param dataPoint Pair of (x,y) values that will be transformed
		* @return Coordinate of the data point in the base image of the plot.
		*/
	ImageCoordinate mDataPointToCoordinate( const DataPointXY& dataPoint );

public:
	/*---Constructors and destructors---*/
	//Plot2D() = default; //Constructs an empty plot with all values set to zero or equivalent

	/**
		* Construct an empty plot of a fixed size.
		* @param width Width of the plot in terms of Pixels
		* @param height Height of the plot in terms of Pixels
		*/
	Plot2D( std::size_t width, std::size_t height );

	/*--------------------------*/

	/*---Getters and Setters----*/

	/**
		* Specify the scaling of the horizontal x-axis of the plot, e.g. linear (default), logarithmic, etc.
		* @param xScalingFunction Function that defines the "new" axis coordinate in terms of the original linear one: x' = xScalingFunction(x)
		*/
	void setXAxisScaling( const std::function<double(double)>& xScalingFunction );

	/**
		* Specify the scaling of the vertical y-axis of the plot, e.g. linear (default), logarithmic, etc.
		* @param yScalingFunction Function that defines the "new" axis coordinate in terms of the original linear one: y' = yScalingFunction(x)
		*/
	void setYAxisScaling( const std::function<double(double)>& yScalingFunction );

	/**
		* Set the range of values represented on the horizontal (x) and vertical (y) axes
		* @param xPlotRange Pair of minimal and maximal values represented on the x axis.
		* @param yPlotRange Pair of minimal and maximal values represented on the y axis.
		*/
	void setPlotRange( const std::pair<double,double>& xPlotRange, const std::pair<double,double>& yPlotRange );

	/*--------------------------*/

	/*---------Modifiers--------*/
	/**
		* Adds a vertical axis to the plot.
		* @param hPos Horizontal coordinate of the axis
		* @param vPosStart Vertical coordinate of the start point of the axis
		* @param vPosEnd Vertical coordinate of the end point of the axis
		* @param color Color of the axis
		*/
	void addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color );

	/**
		* Adds a horizontal axis to the plot.
		* @param vPos vertical coordinate of the axis
		* @param hPosStart Horizontal coordinate of the start point of the axis
		* @param hPosEnd Horizontal coordinate of the end point of the axis
		* @param color Color of the axis
		*/
	void addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color );

	/**
		* Adds a labelled vertical axis to the plot. The position of the axis will be changed to make space for the label.
		* @param hPos Horizontal coordinate of the axis
		* @param vPosStart Vertical coordinate of the start point of the axis
		* @param vPosEnd Vertical coordinate of the end point of the axis
		* @param color Color of the axis (not of label or ticks)
		* @param label Label that is added to the axis
		* @param relativeLabelPosition Position of the first character of label along the axis; has to be between 0.0 and 1.0 (otherwise set to 0.0)
		* @param labelLeft Placement relative to the axis (true: left, false: right)
		* @param rotateLabel If true: rotate the label by 90 degrees clockwise. This can be useful to save space in the plot.
		* @see addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color )
		*/
	void addVerticalAxis( std::size_t hPos, std::size_t vPosStart, std::size_t vPosEnd, T color, const std::string& label, float relativeLabelPosition = 0.5, bool labelLeft = true, bool rotateLabel = false );

	/**
		* Adds a labelled horizontal axis to the plot. The position of the axis will be changed to make space for the label.
		* @param vPos Horizontal coordinate of the axis
		* @param hPosStart Vertical coordinate of the start point of the axis
		* @param hPosEnd Vertical coordinate of the end point of the axis
		* @param color Color of the axis (not of label or ticks)
		* @param label Label that is added to the axis
		* @param relativeLabelPosition Position of the first character of label along the axis; has to be between 0.0 and 1.0 (otherwise set to 0.0)
		* @param labelBelow Placement relative to the axis (true: below, false: above)
		* @see addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color )
		*/
	void addHorizontalAxis( std::size_t vPos, std::size_t hPosStart, std::size_t hPosEnd, T color, const std::string& label, float relativeLabelPosition = 0.5, bool labelBelow = true );

	/**
		* Adds text (string) to the plot.
		* @param text Text that will be added
		* @param textPosition Position where the first character of text will appear in the plot
		*/
	void addText( const std::string& text, ImageCoordinate& textPosition );

	/**
		* Add a data set to the plot with specified plot markers.
		* @param dataSet Pointer to the data set (vector of (x,y)-pairs of double values)
		* @param plotMarker Pixel that will be used to mark the data points from the set in the plot
		*/
	void addDataSet( std::shared_ptr<const DataSet> dataSet, Pixel<T,std::string> plotMarker );

	//"REMOVERS"
	//void flush(); //Removes all data and data points

	/*--------------------------*/
};

/*Include method definitions for the template class*/
#include "../src/plot2d.cpp"
