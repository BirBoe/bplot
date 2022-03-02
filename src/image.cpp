#include "../header/image.h"

/*------------------------------------------------------------------*/
template <typename T1, typename T2>
void Image<T1,T2>::verifyCoordinate(std::size_t i, std::size_t j) const
{
    if( i >= mWidth || j >= mHeight ) {
        throw std::out_of_range("");
    }
}

/*---Constructors and destructors---*/
template <typename T1, typename T2>
Image<T1,T2>::Image()
    : Image(1,1)
{
}

template <typename T1, typename T2>
Image<T1,T2>::Image(std::size_t width, std::size_t height)
    : mWidth(width)
    , mHeight(height)
{
    //CORRECT ORDER of width&height w.r.t. efficiency?
    mPixels = std::vector< std::vector< Pixel<T1,T2> > >( height, std::vector< Pixel<T1,T2> >(width) );
}

template <typename T1, typename T2>
Image<T1,T2>::Image(std::size_t width, std::size_t height, const Pixel<T1,T2> &p)
    : mWidth(width)
    , mHeight(height)
{
    setPixels(p);
}

/*----------------------------------*/


/*-------Getters and Setters--------*/
template <typename T1, typename T2>
void Image<T1,T2>::setAspectRatio( float aspectRatio ) {
    mAspectRatio = aspectRatio;
}

template <typename T1, typename T2>
float Image<T1,T2>::getAspectRatio() const {
    return mAspectRatio;
}

template <typename T1, typename T2>
const Pixel<T1,T2>& Image<T1,T2>::at( std::size_t i, std::size_t j ) const {
    verifyCoordinate(i,j);
    return mPixels.at(j).at(i);
}

template <typename T1, typename T2>
Pixel<T1,T2>& Image<T1,T2>::at( std::size_t i, std::size_t j ) {
    /*Scott Meyer's const_cast: forward call to const version of at() above
     * (cast *this to a const Image& using std:as_const,
     * call const at() and remove const using a const_cast())
     */
     return const_cast< Pixel<T1,T2>& >( std::as_const(*this).at(i,j) );
}

template <typename T1, typename T2>
const std::vector< std::vector< Pixel<T1,T2> > >& Image<T1,T2>::getPixels() const {
    return mPixels;
}

template <typename T1, typename T2>
void Image<T1,T2>::setPixels( const Pixel<T1,T2> &p ) {
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
    mPixels.resize( mHeight, std::vector< Pixel<T1,T2> >(mWidth) );
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
        row.resize( mWidth );
    }
}

/*----------------------------------*/

/*------Other member functions-------*/
template <typename T1, typename T2>
void Image<T1,T2>::show() const
{
    int aR = static_cast<int>( mAspectRatio + 0.5 ); //Round up mAspectRatio
    for( auto& row : getPixels() ) {
        for( auto& pixel : row ) {
            std::cout << pixel;
            for( int k = 2; k <= aR; k++ ) {
                if( mFillSpaces ) std::cout << pixel;
                else std::cout << ' ';
            }
        }
        std::cout << std::endl;
    }
}

/*----------------------------------*/
