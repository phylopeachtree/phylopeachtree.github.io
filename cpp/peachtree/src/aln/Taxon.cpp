/*
 * Taxon.cpp
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */

#include "Taxon.h"

Taxon::Taxon(int id, string name) {
	this->id = id;
	this->name = name;
	this->isHighlighted = false;
	this->isSelected = false;
	this->numInfections = -1;
}



int Taxon::getID() {
	return id;
}

void Taxon::setName(string name) {
	this->name = name;
}

string Taxon::getName() {
	return name;
}


string Taxon::getValue(string var){
	if (this->annotations.find(var) == this->annotations.end()) return "";
	return this->annotations[var];
}

void Taxon::setValue(string var, string val){
	this->annotations[var] = val;
}

void Taxon::toggleSelection() {
	this->isSelected = !this->isSelected;
}

void Taxon::setIsSelected(bool val) {
	this->isSelected = val;
}

bool Taxon::getIsSelected() {
	return this->isSelected;
}


void Taxon::toggleHighlighting() {
	this->isHighlighted = !this->isHighlighted;
}


void Taxon::setIsHighlighted(bool val) {
	this->isHighlighted = val;
}

bool Taxon::getIsHighlighted() {
	return this->isHighlighted;
}

void Taxon::setInfectionCount(double numInfections){
	this->numInfections = numInfections;
}

double Taxon::getInfectionCount(){
	return this->numInfections;
}


