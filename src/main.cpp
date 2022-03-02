#include <cmath> //Basic mathematical functions
#include <cstddef> //std::size_t
#include <iostream>
#include <memory> //smart pointers
#include <stdexcept> //exceptions

#include "../header/main.h"
#include "../header/image.h"
#include "../header/pixel.h"
#include "../header/plot2d.h"

#include "../header/color_models.h"

const std::string FULL_BLOCK = "\u2588";

int main( int argc, char** argv ) {

//Background color : "\033[48" instead

	Pixel<RGB,std::string> p;
	RGB c;
	c.r = 255;
	c.g = 0;
	c.b = 0;
	p.setColor(c);
	//p.setSymbol(FULL_BLOCK);
	p.setSymbol("x");

	Pixel<RGB,std::string> q;
	c.r = 0;
	c.g = 255;
	c.b = 0;
	q.setColor(c);
	//p.setSymbol(FULL_BLOCK);
	q.setSymbol("+");

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

	auto data = std::make_shared<std::vector<std::pair<double,double>>>(10);
	auto data2 = std::make_shared<std::vector<std::pair<double,double>>>(20);
	//std::vector<std::pair<double,double>> data(10);
	for( int i = 0; i < 20; i++ ) {
		if(i<10) data->at(i) = std::make_pair( i, 0.3*(i)*(i) );
		data2->at(i) = std::make_pair( i, i );
	}

	Plot2D<RGB> plt(150,25);
	plt.addDataSet(data2,q);
	plt.addDataSet(data,p);
	//plt.setYAxisScaling( [](double y){ return log10(y); } );
	//plt.setPlotRange( std::make_pair(1.,15.), std::make_pair(1.,27.) );
	plt.show();

	try {
	} catch (const std::out_of_range& e) {
		std::cout << e.what() << std::endl;
	}

	return 0 ;
}
