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

#include "../header/image.h"

/*---------------------Private member functions---------------------*/

template <typename T1, typename T2>
void Image<T1,T2>::verifyCoordinate( const ImageCoordinate& coord ) const
{
    //std::cout << coord.first << "," << mWidth << " - " << coord.second << "," << mHeight << std::endl;
    if( coord.first >= mWidth || coord.second >= mHeight ) {
        throw std::out_of_range("");
    }
}

/*------------------------------------------------------------------*/

/*------------------Constructors and destructors--------------------*/

template <typename T1, typename T2>
Image<T1,T2>::Image()
    : Image(1,1)
{
}

template <typename T1, typename T2>
Image<T1,T2>::Image( std::size_t width, std::size_t height )
    : mWidth(width)
    , mHeight(height)
{
    mPixels = std::vector< std::vector< Pixel<T1,T2> > >( height, std::vector< Pixel<T1,T2> >(width) );
}

template <typename T1, typename T2>
Image<T1,T2>::Image( std::size_t width, std::size_t height, const Pixel<T1,T2> &p )
    : mWidth(width)
    , mHeight(height)
{
    setAllPixels(p);
}

/*------------------------------------------------------------------*/


/*----------------------Getters and Setters-------------------------*/

template <typename T1, typename T2>
void Image<T1,T2>::setAspectRatio( float aspectRatio ) {
    mAspectRatio = aspectRatio;
}

template <typename T1, typename T2>
float Image<T1,T2>::getAspectRatio() const {
    return mAspectRatio;
}

template <typename T1, typename T2>
const Pixel<T1,T2>& Image<T1,T2>::at( const ImageCoordinate& coord ) const {
    verifyCoordinate(coord); //Throws std::out_of_range if coord is invalid
    return mPixels.at(coord.second).at(coord.first);
}

template <typename T1, typename T2>
Pixel<T1,T2>& Image<T1,T2>::at( const ImageCoordinate& coord ) {
    /*Scott Meyer's const_cast: forward call to const version of at() above
     * (cast *this to a const Image& using std:as_const,
     * call const at() and remove const using a const_cast())
     */
     return const_cast< Pixel<T1,T2>& >( std::as_const(*this).at(coord) );
}

template <typename T1, typename T2>
const std::vector< std::vector< Pixel<T1,T2> > >& Image<T1,T2>::getPixels() const {
    return mPixels;
}

template <typename T1, typename T2>
void Image<T1,T2>::setAllPixels( const Pixel<T1,T2> &p ) {
    mPixels = std::vector< std::vector< Pixel<T1,T2> > >( getHeight(), std::vector< Pixel<T1,T2> >(getWidth(),p) );
}

template <typename T1, typename T2>
bool Image<T1,T2>::getFillSpaces() const {
    return mFillSpaces;
}

template <typename T1, typename T2>
void Image<T1,T2>::setFillSpaces( bool fillSpaces ) {
    mFillSpaces = fillSpaces;
}

template <typename T1, typename T2>
std::size_t Image<T1,T2>::getHeight() const
{
    return mHeight;
}

template <typename T1, typename T2>
void Image<T1,T2>::setHeight( std::size_t height )
{
    mHeight = height;
    mPixels.resize( height, std::vector< Pixel<T1,T2> >(getWidth()) );
}

template <typename T1, typename T2>
std::size_t Image<T1,T2>::getWidth() const
{
    return mWidth;
}

template <typename T1, typename T2>
void Image<T1,T2>::setWidth( std::size_t width )
{
    mWidth = width;
    for( auto& row : mPixels ) {
        row.resize( width );
    }
}

/*------------------------------------------------------------------*/

/*-------------------------Other methods----------------------------*/

template <typename T1, typename T2>
void Image<T1,T2>::show() const
{
    int roundedAspectRatio = static_cast<int>( mAspectRatio + 0.5 ); //Round up mAspectRatio
    for( auto& row : getPixels() ) {
        for( auto& pixel : row ) {
            std::cout << pixel;

            /* If the aspect ratio is larger than 1: "Stretch" the image
             * by either outputting the pixel again or insert a spacing */
            for( int k = 2; k <= roundedAspectRatio; k++ ) {
                if( mFillSpaces ) std::cout << pixel;
                else std::cout << ' ';
            }
        }
        std::cout << std::endl;
    }
}

template <typename T1, typename T2>
void Image<T1,T2>::refresh( ) const
{
    refresh( getHeight() );
}

template <typename T1, typename T2>
void Image<T1,T2>::refresh( std::size_t oldHeight ) const
{
    //Set the cursor back to the initial position as defined by the method's arguments
    while( oldHeight > 0 )
    {
    	std::cout << "\e[1A\e[K"; //\e[nA goes n lines up, \e[K clears the line
        oldHeight--;
    }

    //Output the image, i.e. overwrite the old version in the command line
    show();
}

/*------------------------------------------------------------------*/
