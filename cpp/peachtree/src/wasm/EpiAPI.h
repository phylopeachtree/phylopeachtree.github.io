/*
 * EpiAPI.h
 *
 *  Created on: 14/07/2021
 *      Author: jdou557
 */

#ifndef WASM_EPIAPI_H_
#define WASM_EPIAPI_H_

#include <iostream>
#include "../epi/Epidemiology.h"
#include "../aln/Alignment.h"
#include "../aln/Filtering.h"
#include "../phy/Tree.h"
#include "../lib/json.hpp"
using namespace std;
using jsonObject = nlohmann::json;


class EpiAPI {
public:
	static void setEpiAccessionsToDirty();
	static void setEpiAnnotationsToDirty();
	static void validateAccessions(Alignment* alignment);
	static void addAnnotationsToTree(Tree* tree);
	static vector<string> getAllAnnotations();


	static Epidemiology* EPIDEMIOLOGY;
	static bool epiAccessionsAreDirty;
	static bool epiAnnotationsAreDirty;


};

#endif /* WASM_EPIAPI_H_ */