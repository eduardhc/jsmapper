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
 * \file log.h
 * \brief Implementation file for Log class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <unistd.h>

namespace jsmapper
{
	Log * /*static*/ Log::g_theLog = NULL;
	
	static const char * g_levelText[] = 
	{
	    "NONE", 
	    "FATL", 
	    "ERROR", 
	    "WARN", 
	    "INFO", 
	    "DEBG", 
	};
	
	
	class Log::Private
	{
	public:
		Level level;
		FILE  * stream;
		
	public:
		Private()
		    : level( ERROR ), 
		      stream( stdout )
		{
		}
	};

	
	//


	Log::Log()
	{
		d = new Private();
	}
	
	Log * /*static*/ Log::getLog()
	{
		if( g_theLog == NULL )
		{
			g_theLog = new Log();
		}
		
		return g_theLog;
	}

	
	//
	
	
	void Log::setLogLevel( Level level )
	{
		d->level = level;
	}
	
	Log::Level Log::getLogLevel() const
	{
		return d->level;
	}
	
	
	//	
	
	void Log::log( Level level, const char * msg, ... )
	{
		va_list args;
		va_start( args, msg );
		
		if( level <= d->level )
		{
			fprintf( d->stream, "[libjsmapper/%s] ", g_levelText[ level ] );
			vfprintf( d->stream, msg, args );
			fputs( "\n", d->stream );
			fflush( d->stream );
		}
		
		va_end( args );
	}
	
}

