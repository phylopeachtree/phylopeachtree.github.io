/*
 * Option.cpp
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include "Option.h"


Option::~Option() {
	// TODO Auto-generated destructor stub
}


/**
 * Unhide the option
 */
void Option::show() {
	this->hidden = false;
}

/**
 * Hide the option
 */
void Option::hide() {
	this->hidden = true;
}


jsonObject Option::toJSON(){
	jsonObject j;
	return j;
}

string Option::getName(){
	return name;
}

string Option::getSection(){
	return section;
}

string Option::getTitle(){
	return title;
}

jsonObject Option::modifyJSON(jsonObject json){
	json["name"] = name;
	json["section"] = section;
	json["title"] = title;
	json["hide"] = hidden;
	return json;
}

