/*
 * WasmAPI.cpp
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */


#include "WasmAPI.h"

#include <emscripten.h>
#include <string>
#include <iostream>
#include "../lib/json.hpp"
#include "../error/Error.h"
using json = nlohmann::json;

using namespace std;


char* WasmAPI::heap;
int WasmAPI::currentMsgID = -1;

void WasmAPI::messageFromWasmToJS(const string & msg) {


	int msgID = WasmAPI::getCurrentMsgID();
	WasmAPI::currentMsgID = -1;
	//cout << "msg " << msgID << endl;

	if (msgID < 0) return;

	//if (msg.size() > 1000) cout << "Parsing string of size " << msg.size() << " msgID " << msgID << endl;

	EM_ASM_ARGS({
    	var msg = UTF8ToString($0); // Convert message to JS string
    	messageFromWasmToJS(msg, $1);
  	}, msg.c_str(), msgID);

}



char* WasmAPI::getFromHeap(){
	cout << "Getting from heap " << strlen(WasmAPI::heap) << endl;
	char* toReturn = WasmAPI::heap;
	heap = NULL;
	return toReturn;
}


void WasmAPI::putOnHeap(char* str){
	cout << "Adding to heap " << strlen(str) << endl;
	//if (strlen(heap) > 0) delete heap;
	heap = str;
}

void WasmAPI::returnErrorJSON(string error){

	std::cout << "Error: " << error << endl;


	json j;
	j["err"] = error;
	WasmAPI::messageFromWasmToJS(j.dump(0));

	//delete j;

}



void WasmAPI::setCurrentMsgID(int msgID){
	WasmAPI::currentMsgID = msgID;
	//cout << "setting msgID to " << WasmAPI::currentMsgID << endl;
}

int WasmAPI::getCurrentMsgID(){
	return WasmAPI::currentMsgID;
}


// Interface between javascript and cpp for webassembly
extern "C" {


	/*
	void EMSCRIPTEN_KEEPALIVE test(char* str, int x, int msgID){
        cout << str << " " << x << " " << msgID << endl;


        json j;
		j["pi"] = 3.141;

		//cout << j.dump(4) << endl;

        WasmAPI::messageFromWasmToJS(j.dump(0), msgID);
	}
	*/

	void EMSCRIPTEN_KEEPALIVE putOnHeap(char* str){
		WasmAPI::putOnHeap(str);
	}

	void EMSCRIPTEN_KEEPALIVE setCurrentMsgID(int msgID){
		WasmAPI::setCurrentMsgID(msgID);
		Error::clear();
	}


}


