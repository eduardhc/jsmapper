#ifndef __JSMAPPER_CONFIG_H_
#define __JSMAPPER_CONFIG_H_

/**
    \brief Template configuration file for JSMapper
    \author Eduard Huguet <eduardhc@gmail.com>

    This file serves as a template for the JSMapper generated configuration file. It's used to generate the actual config.h 
    file containing actual compiling options, which are determined at cmake-execution time.
 */


/**
    \brief Install prefix
 */

#cmakedefine JSMAPPER_INSTALL_PREFIX "@JSMAPPER_INSTALL_PREFIX@"



#endif


