/*
 * JalView.cpp
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */

#include "JalView.h"

JalView::JalView() {
	colours["A"] = "#64F73F";
	colours["C"] = "#FFB340";
	colours["G"] = "#EB413C";
	colours["T"] = "#3C88EE";
	colours["U"] = "#3C88EE";
}




string JalView::getName(){
	return "JalView";
}


bool JalView::isNucleotide(){
	return true;
}
