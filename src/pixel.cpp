#include "../header/pixel.h"

/*---Constructors and destructors---*/
template <typename T1, typename T2>
Pixel<T1,T2>::Pixel( T1 color, T2 symbol )
    : mColor(color)
    , mSymbol(symbol)
{
}

/*----------------------------------*/


/*-------Getters and Setters--------*/
template <typename T1, typename T2>
T1 Pixel<T1,T2>::getColor() const
{
    return mColor;
}

template <typename T1, typename T2>
void Pixel<T1,T2>::setColor( T1 color )
{
    mColor = color;
}

template <typename T1, typename T2>
T2 Pixel<T1,T2>::getSymbol() const
{
    return mSymbol;
}

template <typename T1, typename T2>
void Pixel<T1,T2>::setSymbol( T2 symbol )
{
    mSymbol = symbol;
}

/*----------------------------------*/

/*-------Operator--------*/
template <typename T1, typename T2>
std::ostream& operator<<( std::ostream& ostr, const Pixel<T1,T2>& p )
{
    ostr << color_to_ansi( p.getColor() ) << p.getSymbol() << "\033[0m"; //"\033[0m" = RESET
    return ostr;
}
/*-----------------------*/
