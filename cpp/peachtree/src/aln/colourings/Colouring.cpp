/*
 * Colouring.cpp
 *
 *  Created on: 6/07/2021
 *      Author: jdou557
 */

#include "../colourings/Colouring.h"
#include "../Filtering.h"


void Colouring::init(){
	this->colFilter = SiteColourFilter::minor;
}


void Colouring::setSiteColourFilter(SiteColourFilter val, Filtering* filtering){
	this->filtering = filtering;
	this->colFilter = val;
}


void Colouring::setSiteColourFilter(string val, Filtering* filtering){
	SiteColourFilter filterVal  = SiteColourFilter::all;
	if (val == "major") filterVal = SiteColourFilter::major;
	if (val == "minor") filterVal = SiteColourFilter::minor;
	this->setSiteColourFilter(filterVal, filtering);
}




string Colouring::getName(){
	return "null";
}


bool Colouring::isNucleotide(){
	return false;
}

string Colouring::getDefaultCol(){
	return "white";
}



/*
 * Get all values from enum
 * No elegant way of doing this AFAIK
 */
vector<string> Colouring::getSiteColourFilters(){
	vector<string> domain = {"all", "major", "minor"};
	return domain;
}
string Colouring::getDefaultSiteColourFilter(){
	return "minor";
}

string Colouring::getColour(string symbol, int siteNum){


	Filtering* f = this->filtering;

	// Major alleles only
	if (this->colFilter == SiteColourFilter::major) {
		if (!f->isMajorAllele(symbol, siteNum)) return this->getDefaultCol();
	}


	// Minor alleles only
	else if (this->colFilter == SiteColourFilter::minor) {
		if (!f->isMinorAllele(symbol, siteNum)) return this->getDefaultCol();
	}

	// All
	string col = this->colours[symbol];
	if (col == "") return this->getDefaultCol();
	return col;

}

string Colouring::toJSON(){
	json j;
	for (const auto& pair : colours) {
		string symbol = pair.first;
		string colour = colours[symbol];
		j[symbol] = colour;
	}
	return j.dump(0);

}

string Colouring::toString(){
	return this->getName();
}






