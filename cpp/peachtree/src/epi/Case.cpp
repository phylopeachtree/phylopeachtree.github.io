/*
 * Case.cpp
 *
 *  Created on: 14/07/2021
 *      Author: jdou557
 */

#include "Case.h"

Case::Case(string accession, map<string, string> annotations) {
	this->accession = accession;
	this->annotations = annotations;
}


/*
 * Get the value of this variable
 */
string Case::getValue(string var){
	if (this->annotations.find(var) == this->annotations.end()) return "";
	return this->annotations[var];
}


/*
 * Get this accession
 */
string Case::getAccession(){
	return this->accession;
}


/*
 * Get a list of variables
 */
vector<string> Case::getVariables(){
	vector<string> keys;
	for (const auto & pair : this->annotations){
		keys.push_back(pair.first);
	}
	return keys;
}




void Case::setActive(bool val){
	this->active = val;
}


bool Case::getActive(){
	return this->active;
}
