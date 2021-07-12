/*
 * error.h
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */


#include <iostream>
using namespace std;

#ifndef ERROR_ERROR_H_
#define ERROR_ERROR_H_

class Error {
public:
	static void throwError(string msg);
	static bool wasError();
	static void clear();
private:
	static bool thrown;
};

#endif /* ERROR_ERROR_H_ */
