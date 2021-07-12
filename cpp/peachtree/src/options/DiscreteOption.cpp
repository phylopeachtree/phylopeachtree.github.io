/*
 * DiscreteOption.cpp
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include "DiscreteOption.h"
#include "../error/Error.h"


DiscreteOption::DiscreteOption(string name, string section, string title, string value, vector<string> domain){
	this->name = name;
	this->domain = domain;
	this->section = section;
	this->title = title;
	this->hidden = false;
	this->setVal(value);
}


DiscreteOption::DiscreteOption(string name, string section, string title, string value, vector<string> domain, bool hide){
	this->name = name;
	this->domain = domain;
	this->section = section;
	this->title = title;
	this->hidden = hide;
	this->setVal(value);
}


jsonObject DiscreteOption::toJSON() {
	jsonObject json;
	jsonObject arr;
	json["value"] = this->value;
	json["type"] = "DiscreteOption";
	for (string dom : this->domain) arr.push_back(dom);
	json["domain"] = arr;
	json = modifyJSON(json);
	return json;
}



string DiscreteOption::getVal() {
	return this->value;
}

vector<string> DiscreteOption::getDomain(){
	return this->domain;
}

void DiscreteOption::setVal(string value) {
	for (string dom : this->domain){
		if (dom == value){
			this->value = value;
			return;
		}
	}
	Error::throwError("Cannot assign " + this->name + " to " + value);
}










