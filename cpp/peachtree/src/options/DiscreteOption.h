/*
 * DiscreteOption.h
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */


#include "../Utils.h"
#include "Option.h"
#include <iostream>
#include "../lib/json.hpp"
using jsonObject = nlohmann::json;
using namespace std;

#ifndef OPTIONS_DISCRETEOPTION_H_
#define OPTIONS_DISCRETEOPTION_H_



class DiscreteOption : public Option {
public:
	DiscreteOption(string name, string section, string title, string value, vector<string> domain);
	DiscreteOption(string name, string section, string title, string value, vector<string> domain, bool hide);
	void setValAndDomain(string value, vector<string> domain);
	virtual jsonObject toJSON() override;
	string getVal();
	vector<string> getDomain();
	void setVal(string val);
	void cleanup();

private:
	vector<string> domain;
	string value;
};

#endif /* OPTIONS_DISCRETEOPTION_H_ */
