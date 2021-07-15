
#include <cstdlib>
#include "../error/Error.h"
#include "WasmAPI.h"


bool Error::thrown = false;

/*
 * Throw an error using webassembly
 */
void Error::throwError(string msg){
	Error::thrown = true;
	WasmAPI::returnErrorJSON(msg);
	 //std::abort();
}

/*
 * Was an error thrown?
 */
bool Error::wasError(){
	return Error::thrown;
}

/*
 * Clear error
 */
void Error::clear(){
	Error::thrown = false;
}
