/*
 * ClustalAmino.cpp
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */

#include "AlphaAmino.h"

AlphaAmino::AlphaAmino() {


	// A lot of different colours from colour brewer
	colours["A"] = "#762A83";
	colours["B"] = "#7FBC41";
	colours["C"] = "#E08214";
	colours["D"] = "#B2ABD2";
	colours["E"] = "#C51B7D";
	colours["F"] = "#4393C3";
	colours["G"] = "#1B7837";
	colours["H"] = "#35978F";
	colours["I"] = "#D9EF8B";
	colours["J"] = "#2D004B";
	colours["K"] = "#DFC27D";
	colours["L"] = "#543005";
	colours["M"] = "#F46D43";
	colours["N"] = "#5E4FA2";
	colours["O"] = "#B2182B";
	colours["P"] = "#FDDBC7";
	colours["Q"] = "#FDE0EF";
	colours["R"] = "#F46D43";
	colours["S"] = "#E6F598";
	colours["T"] = "#D9F0D3"; 
	colours["U"] = "#4575B4";
	colours["V"] = "#FFFFBF";
	colours["W"] = "#1A9850";
	colours["X"] = "#4D4D4D";
	colours["Y"] = "#F4A582";
	colours["Z"] = "#F46D43";
	
	// White font for the dark bg cols
	textColours["A"] = "white";
	textColours["J"] = "white";
	textColours["L"] = "white";
	textColours["O"] = "white";
	textColours["G"] = "white";
	textColours["X"] = "white";
	textColours["U"] = "white";
	
	

}


string AlphaAmino::getName(){
	return "Alpha";
}


bool AlphaAmino::isNucleotide(){
	return true;
}

bool AlphaAmino::isAlpha(){
	return true;
}



