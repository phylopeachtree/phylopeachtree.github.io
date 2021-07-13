/*
 * error.cpp
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */

#include "Error.h"


bool Error::thrown = false;

/*
 * Throw an error as per usual
 */
void Error::throwError(string msg){
	//Error::thrown = true;
	throw runtime_error(msg);
}


/*
 * Was an error thrown?
 */
bool Error::wasError(){
	return false; //Error::thrown;
}

/*
 * Clear error
 */
void Error::clear(){
	//Error::thrown = false;
}
