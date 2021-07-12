/*
 * Colouring.h
 *
 *  Created on: 6/07/2021
 *      Author: jdou557
 */


#ifndef ALN_COLOURING_COLOURING_H_
#define ALN_COLOURING_COLOURING_H_


#include <iostream>
#include <vector>
#include "../Filtering.h"
#include "../../lib/json.hpp"


using namespace std;
using json = nlohmann::json;





enum class SiteColourFilter {
	all, minor, major
};


class Colouring {
public:


	virtual string getName();
	virtual bool isNucleotide();

	void init();
	void setSiteColourFilter(SiteColourFilter val, Filtering* filtering);
	void setSiteColourFilter(string val, Filtering* filtering);
	string getDefaultCol();
	string getColour(string symbol, int siteNum);
	string toJSON();
	string toString();

	static vector<string> getSiteColourFilters();
	static string getDefaultSiteColourFilter();



protected:
	std::map<string, string> colours;
	SiteColourFilter colFilter;
	Filtering* filtering;

};

#endif /* ALN_COLOURING_COLOURING_H_ */


