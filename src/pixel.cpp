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

/*-------Operators--------*/
template <typename T1, typename T2>
std::ostream& operator<<( std::ostream& ostr, const Pixel<T1,T2>& p )
{
    ostr << color_to_ansi( p.getColor() ) << p.getSymbol() << "\033[0m"; //"\033[0m" = RESET
    return ostr;
}
/*------------------------*/
