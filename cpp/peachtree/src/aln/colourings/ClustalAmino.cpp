/*
 * ClustalAmino.cpp
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */

#include "ClustalAmino.h"

ClustalAmino::ClustalAmino() {

	colours["G"] = "orange";
	colours["P"] = "orange";
	colours["S"] = "orange";
	colours["T"] = "orange";

	colours["H"] = "red";
	colours["K"] = "red";
	colours["R"] = "red";

	colours["F"] = "blue";
	colours["W"] = "blue";
	colours["Y"] = "blue";

	colours["I"] = "green";
	colours["L"] = "green";
	colours["M"] = "green";
	colours["V"] = "green";

}


string ClustalAmino::getName(){
	return "ClustalAmino";
}


bool ClustalAmino::isNucleotide(){
	return false;
}
