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

