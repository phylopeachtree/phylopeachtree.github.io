/*
 * Timeline.h
 *
 *  Created on: 15/07/2021
 *      Author: jdou557
 */

#ifndef EPI_TIMELINE_H_
#define EPI_TIMELINE_H_

#include <time.h>
#include <iostream>
#include <map>
#include "Epidemiology.h"

using namespace std;




class Timeline {
public:
	Timeline(Epidemiology* epidemiology, string sampleDateVariable, string dateFormat);
	void setSampleDateVariable(string sampleDateVariable, string dateFormat);
	void cleanup();


	const static vector<string> dateFormats;
	static string getDefaultDateFormat();

private:
	Epidemiology* epidemiology;
	string sampleDateVariable;
	string dateFormat;




};

#endif /* EPI_TIMELINE_H_ */
