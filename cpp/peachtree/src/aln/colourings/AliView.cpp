/*
 * AliView.cpp
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */

#include "AliView.h"

/*
* https://github.com/AliView/AliView/blob/master/src/main/java/aliview/color/DefaultColorScheme.java
*/
AliView::AliView() {
	colours["A"] = "#5ADC5A";
	colours["C"] = "#6464FA";
	colours["G"] = "#5A5A5A";
	colours["T"] = "#F58282";
	colours["U"] = "#F58282";
	
	textColours["A"] = "#018018";
	textColours["C"] = "#0101FF";
	textColours["G"] = "#000000";
	textColours["T"] = "#FF0000";
	textColours["U"] = "#FF0000";
	
}




string AliView::getName(){
	return "AliView";
}


bool AliView::isNucleotide(){
	return true;
}
