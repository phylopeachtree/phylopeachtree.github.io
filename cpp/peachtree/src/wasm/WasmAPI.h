/*
 * WasmAPI.h
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */

#include <string>
#include <iostream>

using namespace std;

#ifndef WASMAPI_H_
#define WASMAPI_H_


class WasmAPI {
public:
	static void returnErrorJSON(string error);
	static void messageFromWasmToJS(const string & msg);
	static void putOnHeap(char* str);
	static char* getFromHeap();
	static void setCurrentMsgID(int msgID);
	static int getCurrentMsgID();

private:
	static char* heap;
	static int currentMsgID;
};

#endif /* WASMAPI_H_ */
