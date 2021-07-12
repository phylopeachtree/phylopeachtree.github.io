/*
 * BooleanOption.cpp
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include "BooleanOption.h"

BooleanOption::BooleanOption(string name, string section, string title, bool value){
	this->name = name;
	this->section = section;
	this->title = title;
	this->value = value;
	this->hidden = false;
}


BooleanOption::BooleanOption(string name, string section, string title, bool value, bool hidden){
	this->name = name;
	this->section = section;
	this->title = title;
	this->value = value;
	this->hidden = hidden;
}


jsonObject BooleanOption::toJSON() {
	jsonObject json;
	json["value"] = this->value;
	json["type"] = "BooleanOption";
	json = modifyJSON(json);
	return json;
}



bool BooleanOption::getVal() {
	return this->value;
}

void BooleanOption::setVal(bool value) {
	this->value = value;
}

