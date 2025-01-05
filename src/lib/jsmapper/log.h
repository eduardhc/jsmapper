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
 * \brief Declaration file for Log class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __LIBJSMAPPER_LOG_H_
#define __LIBJSMAPPER_LOG_H_

#include <string>

namespace jsmapper
{
	/**
	  \brief Log configuration class
	  
	  Allows configuration of library log feature
	  */
	class Log
	{
	public:
		/**
		  \brief Returns log singleton instance
		  */
		static Log * getLog();
		
	public:
		typedef enum 
		{
			NONE	= 0, 
			FATL	= 1, 
			ERROR	= 2, 
			WARN	= 3, 
			INFO	= 4, 
			DEBG	= 5
		} Level;
		
		/**
		  \brief Sets log level (default is ERROR)
		  */
		void setLogLevel( Level level );
		
		/**
		  \brief Returns current log level
		  */
		Level getLogLevel() const;
		
	
	public:
		/**
		  \brief Logs text, if current log level is equal or superior to given one
		  
		  Don't use this function directly. Use JSMAPPER_LOG_ constants instead
		  */
		void log( Level level, const char * msg, ... );
		
	
	protected:
		/**
		  \brief Default constructor
		  Protected, as it will be instantiated only through singleton access point.
		  */
		Log();
		
	
	protected:
		static Log * g_theLog;
		
	
	private:
		class Private;
		Private * d;
	};
	
}


#define JSMAPPER_LOG( level, msg... )				\
	{ jsmapper::Log::getLog()->log( level, msg ); }

#define JSMAPPER_LOG_FATAL(message...)				JSMAPPER_LOG( jsmapper::Log::FATL, message )
#define JSMAPPER_LOG_ERROR(message...)				JSMAPPER_LOG( jsmapper::Log::ERROR, message )
#define JSMAPPER_LOG_WARNING(message...)			JSMAPPER_LOG( jsmapper::Log::WARN, message )
#define JSMAPPER_LOG_INFO(message...)				JSMAPPER_LOG( jsmapper::Log::INFO, message )
#define JSMAPPER_LOG_DEBUG(message...)				JSMAPPER_LOG( jsmapper::Log::DEBG, message )



#endif //_LOG_H_
