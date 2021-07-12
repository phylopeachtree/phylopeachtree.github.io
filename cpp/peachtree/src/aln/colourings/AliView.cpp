/*
 * AliView.cpp
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */

#include "AliView.h"

AliView::AliView() {
	colours["A"] = "#5ADC5A";
	colours["C"] = "#6464FA";
	colours["G"] = "#5A5A5A";
	colours["T"] = "#F58282";
	colours["U"] = "#F58282";
}




string AliView::getName(){
	return "AliView";
}


bool AliView::isNucleotide(){
	return true;
}
