/*
 * Drums.cpp
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */

#include "Drums.h"

Drums::Drums() {
	colours["A"] = "#5050ff";
	colours["C"] = "#e00000";
	colours["G"] = "#00c000";
	colours["T"] = "#e6e600";
	colours["U"] = "#e6e600";
}


string Drums::getName(){
	return "DRuMS";
}


bool Drums::isNucleotide(){
	return true;
}
