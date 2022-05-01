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

#include <cmath> //Basic mathematical functions
#include <cstddef> //std::size_t
#include <fstream>
#include <iostream>
#include <memory> //smart pointers
#include <stdexcept>

#include "../header/main.h"
#include "../header/image.h"
#include "../header/pixel.h"
#include "../header/plot2d.h"

#include "../header/color_models.h"

//const std::string FULL_BLOCK = "\u2588";

/**
 * Reads a csv file with two columns (x and y) and returns a dataset, i.e. vector of pairs of double values (x,y).
 * @param filename Name of/Path to a csv file with two columns seperated by a comma.
 * @param xFactor Factor that the x values are multiplied with
 * @param yFactor Factor that the y values are multiplied with
 * @return Vector of pairs of double values (x,y) corresponding to the two columns of the file.
 */
DataSet datasetFromCsvFile( std::string_view filename, double xFactor = 1., double yFactor = 1. )
{
	DataSet data;
	std::ifstream file;
	file.open(filename);

	if( !file ) {
		std::cout << "The file " << filename << " could not be opened." << std::endl;
		return data;
	}
	double x, y;
	char c;
	while( !file.eof() && ( file >> x >> c >> y && c == ',' )) {
		data.push_back( std::make_pair(x*xFactor,y*yFactor) );
	}

	file.close();

	return data;
}

int main( int argc, char** argv ) {
	/*---- Interpret the arguments ----*/
	std::vector<std::string> filenames; //Default CSV-File from which (x,y)-values will be read

	int argCtr = 1;
	while( argCtr < argc ) {
		if( std::string(argv[argCtr]) == "--file" || std::string(argv[argCtr]) == "-f" ) {
				filenames.push_back(argv[argCtr+1]);
				argCtr += 2;
		} else {
			argCtr++;
		}
	}

	if( filenames.size() == 0 ) {
		filenames.push_back( "testdata.csv" ); //Default CSV-File from which (x,y)-values will be read if no arguments are given
	}

	/*---- Define four different Pixels as plot markers -----*/

	std::vector< Pixel<RGB,std::string> > plotMarkers(4);
	RGB c;
	c.r = 255;
	c.g = 0;
	c.b = 0;
	plotMarkers.at(0).setColor(c);
	plotMarkers.at(0).setSymbol("x");

	c.r = 0;
	c.g = 255;
	plotMarkers.at(1).setColor(c);
	plotMarkers.at(1).setSymbol("+");

	c.g = 0;
	c.b = 255;
	plotMarkers.at(2).setColor(c);
	plotMarkers.at(2).setSymbol("o");

	c.r = 255;
	c.g = 255;
	c.b = 255;
	plotMarkers.at(3).setColor(c);
	plotMarkers.at(3).setSymbol("#");

	/*---- Generate a plot of fixed size ---- */
	Plot2D<RGB> plt(150,25,"X","Y");

	/*---- Load all data sets from files and add to the plot ----*/
	int ctr = 0;
	for( auto& filename : filenames ) {
		auto dataFromFile = datasetFromCsvFile( filename );
		auto data = std::make_shared<std::vector<std::pair<double,double>>>(dataFromFile);
		plt.addDataSet(data,plotMarkers.at(ctr%4));
		ctr++;
	}

	/*---- Show the plot in the command line ---- */
	plt.show();



	/*****************************/
	/* --- BEGIN OF TEST AREA--- */
	/*****************************/

	//Image<Mono,char> img(10,10);
	/*Image<RGB,std::string> img(10,10,p); //Equivalent: Image<> = Image<Mono,char>
	c.r = 0;
	c.g = 255;
	p.setColor(c);
	p.setSymbol("x");
	img.at(2,5) = p;
	img.setFillSpaces(true);
	img.setAspectRatio(3.);*/

	//img.show();

	//auto data = std::make_shared<std::vector<std::pair<double,double>>>(10);
	//auto data2 = std::make_shared<std::vector<std::pair<double,double>>>(20);
	/*for( int i = 0; i < 20; i++ ) {
		if(i<10) data->at(i) = std::make_pair( i, 0.3*(i)*(i) );
		data2->at(i) = std::make_pair( i, i );
	}*/

	//plt.setYAxisScaling( [](double y){ return log10(y); } );
	//plt.setPlotRange( std::make_pair(1.,15.), std::make_pair(1.,27.) );

	/*****************************/
	/* ---- END OF TEST AREA---- */
	/*****************************/

	return 0 ;
}
