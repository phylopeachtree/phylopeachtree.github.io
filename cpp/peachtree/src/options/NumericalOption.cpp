/*
 * NumericalOption.cpp
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include "NumericalOption.h"


NumericalOption::NumericalOption(string name, string section, string title, double val, double min, double max, double stepSize){
	this->name = name;
	this->min = min;
	this->max = max;
	this->value = val;
	this->section = section;
	this->title = title;
	this->longTitle = title;
	this->stepSize = stepSize;
	this->hidden = false;
	this->defaultVal = val;
}


NumericalOption::NumericalOption(string name, string section, string title, double val, double min, double max, double stepSize, bool hidden){
	this->name = name;
	this->min = min;
	this->max = max;
	this->value = val;
	this->section = section;
	this->title = title;
	this->longTitle = title;
	this->stepSize = stepSize;
	this->hidden = hidden;
	this->defaultVal = val;
}



jsonObject NumericalOption::toJSON() {
	jsonObject json;
	json["value"] = value;
	json["min"] = min;
	json["max"] = max;
	json["step"] = stepSize;
	json["type"] = "NumericalOption";
	json = modifyJSON(json);
	return json;
}


void NumericalOption::resetToDefault(){
	this->setVal(this->defaultVal);
}

double NumericalOption::getStepSize(){
	return this->stepSize;
}


double NumericalOption::getVal() {
	return this->value;
}


double NumericalOption::getMin(){
	return this->min;
}

double NumericalOption::getMax(){
	return this->max;
}

void NumericalOption::setVal(double val) {
	//if (Double.isNaN(val)) return;
	if (val <= this->min) val = this->min;
	if (val >= this->max) val = this->max;
	this->value = val;
}




