/*
 * Timeline.cpp
 *
 *  Created on: 15/07/2021
 *      Author: jdou557
 */

#include "Timeline.h"

const vector<string> Timeline::dateFormats({"yyyy-mm-dd", "dd-mm-yyyy", "mm-dd-yyyy"});


Timeline::Timeline(Epidemiology* epidemiology, string sampleDateVariable, string dateFormat) {
	this->epidemiology = epidemiology;
	this->setSampleDateVariable(sampleDateVariable, dateFormat);
}


void Timeline::setSampleDateVariable(string sampleDateVariable, string dateFormat){

	// Nothing has changed
	if (this->sampleDateVariable == sampleDateVariable && this->dateFormat == dateFormat) return;


	// Check the variable name is valid
	vector<string> annots = this->epidemiology->getAnnotations();
	if (std::count(annots.begin(), annots.end(), sampleDateVariable) == 0) return;


	// Check the date format is valid
	if (std::count(Timeline::dateFormats.begin(), Timeline::dateFormats.end(), dateFormat) == 0) return;


	// Get a unique list of (sample) dates
	vector<string> dates;
	for (Case* c : epidemiology->getCases()){
		string val = c->getValue(sampleDateVariable);
		if (val == "") continue;

		// Unique dates only
		if (std::count(dates.begin(), dates.end(), val) == 0) {
			dates.push_back(val);
		}
	}


	// Set variable
	this->sampleDateVariable = sampleDateVariable;
	this->dateFormat = dateFormat;


}




/*
 * Default
 */
string Timeline::getDefaultDateFormat(){
	return Timeline::dateFormats.at(0);
}



/*
 * Cleanup
 */
void Timeline::cleanup(){


}
