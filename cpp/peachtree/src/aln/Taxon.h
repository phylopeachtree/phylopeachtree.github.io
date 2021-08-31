/*
 * Taxon.h
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */

#ifndef ALN_TAXON_H_
#define ALN_TAXON_H_


#include <iostream>
#include <map>
#include "../epi/Case.h"

using namespace std;


class Taxon {
public:
	Taxon(int id, string name);
	int getID();
	void setName(string name);
	string getName();
	void toggleSelection();
	void setIsSelected(bool val);
	bool getIsSelected();
	void toggleHighlighting();
	void setIsHighlighted(bool val);
	bool getIsHighlighted();
	string getValue(string var);
	void setValue(string var, string val);
	void setInfectionCount(double numInfections);
	double getInfectionCount();
	void setCase(Case* casePatient);
	Case* getCase();


private:
	int id;
	string name;
	bool isSelected;
	bool isHighlighted;
	map<string, string> annotations;
	double numInfections;
	Case* casePatient;


};

#endif /* ALN_TAXON_H_ */
