/**
 * Copyright 2013 Eduard Huguet Cuadrench
 * 
 * This file is part of JSMapper Library.
 * 
 * JSMapper Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with JSMapper Library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * \file band.h
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \brief Definition for Band class
 */

#ifndef __LIBJSMAPPER_BAND_H_
#define __LIBJSMAPPER_BAND_H_

#include "common.h"

namespace jsmapper
{

	/**
	  \brief Axis band definition

	  This class is used to represent an axis "band", this is, a range of axis values for which
	  we'll be able to assign an action
	  */
	class Band
	{
	public:
		/// lower value for band
		int m_low;
		/// higher value for band
		int m_high;

	public:
		Band( int low = -65535, int high = 65535 )	// TODO use MAXINT constants...
			: m_low( low ),
			  m_high( high )
		{
		}

	public:
		bool operator == ( const Band &other ) const
		{
			return ( m_low == other.m_low && m_high == other.m_high );
		}
	};

}

#endif // __LIBJSMAPPER_BAND_H_
