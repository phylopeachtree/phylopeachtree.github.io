/*
 * Epidemiology.h
 *
 *  Created on: 14/07/2021
 *      Author: jdou557
 */

#ifndef EPI_EPIDEMIOLOGY_H_
#define EPI_EPIDEMIOLOGY_H_


#include <iostream>
#include "Case.h"
#include "../aln/Alignment.h"
#include "../phy/Tree.h"
#include "../lib/json.hpp"
using namespace std;
using jsonObject = nlohmann::json;



class Epidemiology {
public:
	Epidemiology();
	void parseFile(string contents);
	vector<string> getAnnotations();
	void addAnnotationsToTree(Tree* tree);
	void clearAnnotationsFromTree(Tree* tree);
	void validateAccessions(Alignment* alignment);
	vector<Case*> getCases();
	void cleanup();

private:
	vector<Case*> cases;
	vector<string> annotations;
	
};

#endif /* EPI_EPIDEMIOLOGY_H_ */
