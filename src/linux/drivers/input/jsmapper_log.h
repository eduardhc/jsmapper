/**
 * Copyright 2013 Eduard Huguet Cuadrench
 * 
 * This file is part of JSMapper kernel module.
 * 
 * JSMapper is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with JSMapper.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * \file jsmapper_log.h
 * \brief JSMapper kernel module log macros
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \todo Select debug level at runtime
 */

#ifndef __JSMAPPER_LOG_H_
#define __JSMAPPER_LOG_H_

#define JSMAPPER_LOG_DEBUG( _MSG_...)           JSMAPPER_LOG( "(DBG)", _MSG_ )
#define JSMAPPER_LOG_INFO( _MSG_...)            JSMAPPER_LOG( "(INF)", _MSG_ )
#define JSMAPPER_LOG_WARNING( _MSG_...)         JSMAPPER_LOG( "(WRN)", _MSG_ )
#define JSMAPPER_LOG_ERROR( _MSG_...)           JSMAPPER_LOG( "(ERR)", _MSG_ )

#define JSMAPPER_LOG( _LEVEL_, _MSG_...)        \
            { printk( "jsmapper " _LEVEL_ ": " _MSG_ ); printk( "\n" ); }


#endif // __JSMAPPER_LOG_H_
