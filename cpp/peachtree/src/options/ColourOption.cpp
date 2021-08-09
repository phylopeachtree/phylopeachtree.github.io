/*
 * ColourOption.cpp
 *
 *  Created on: 9/08/2021
 *      Author: jdou557
 */

#include "ColourOption.h"
#include "../Utils.h"


ColourOption::ColourOption(string name, string section, string title){
	this->name = name;
	this->section = section;
	this->title = title;
	this->hidden = false;

	initCols();
	this->setVal(this->colours.at(0));
}


ColourOption::ColourOption(string name, string section, string title, string value){
	this->name = name;
	this->section = section;
	this->title = title;
	this->hidden = false;

	initCols();
	this->setVal(value);
}


ColourOption::ColourOption(string name, string section, string title, string value, bool hidden){
	this->name = name;
	this->section = section;
	this->title = title;
	this->hidden = hidden;

	initCols();
	this->setVal(value);
}


void ColourOption::initCols(){

	this->colours.clear();
	this->colours.push_back("#000000"); // black
	this->colours.push_back("#242424"); // dark grey
	this->colours.push_back("#444444"); // dark/med grey
	this->colours.push_back("#696969"); // med grey
	this->colours.push_back("#999999"); // med/light grey
	this->colours.push_back("#d3d3d3"); // light grey


	this->colours.push_back("#ba2e00"); // dark red
	this->colours.push_back("#ff0b08"); // red
	this->colours.push_back("#ff8608"); // orange
	this->colours.push_back("#fdff08"); // yellow
	this->colours.push_back("#8bbb00"); // light green
	this->colours.push_back("#00ba2e"); // green
	this->colours.push_back("#008cba"); // blue
	this->colours.push_back("#002ebb"); // dark blue
	this->colours.push_back("#29465b"); // dark blue grey
	this->colours.push_back("#FFDAB9"); // peach
	this->colours.push_back("#ba008c"); // magenta
	this->colours.push_back("#8e00bb"); // purple




}


jsonObject ColourOption::toJSON() {
	jsonObject json;
	jsonObject arr;
	json["value"] = this->value;
	json["value"] = this->value;
	json["type"] = "ColourOption";
	for (int i = 0; i < this->colours.size(); i ++){
		string col = this->colours.at(i);
		arr.push_back(col);
	}
	json["colours"] = arr;

	json = modifyJSON(json);
	return json;

}



string ColourOption::getVal() {
	return this->value;
}

void ColourOption::setVal(string value) {
	value = Utils::toLower(value);
	for (int i = 0; i < this->colours.size(); i ++){
		string col = Utils::toLower(this->colours.at(i));
		if (col == value){
			this->value = value;
			return;
		}
	}
	this->value = this->colours.at(0);
}



