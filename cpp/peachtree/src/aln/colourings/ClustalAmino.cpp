/*
 * ClustalAmino.cpp
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */

#include "ClustalAmino.h"

ClustalAmino::ClustalAmino() {

	colours["A"] = "#80a0f0";
	colours["I"] = "#80a0f0";
	colours["L"] = "#80a0f0";
	colours["M"] = "#80a0f0";
	colours["F"] = "#80a0f0";
	colours["W"] = "#80a0f0";
	colours["V"] = "#80a0f0";
	colours["C"] = "#80a0f0";

	colours["K"] = "#f01505";
	colours["R"] = "#f01505";
	
	colours["E"] = "#c048c0";
	colours["D"] = "#c048c0";
	
	colours["N"] = "#15c015";
	colours["Q"] = "#15c015";
	colours["S"] = "#15c015";
	colours["T"] = "#15c015";

	colours["C"] = "#f08080";
	
	colours["G"] = "#f09048";
	
	colours["P"] = "#c0c000";
	
	colours["H"] = "#15a4a4";
	colours["Y"] = "#15a4a4";

}


string ClustalAmino::getName(){
	return "ClustalX";
}


bool ClustalAmino::isAlpha(){
	return true;
}
