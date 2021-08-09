/*
 * ColourOption.h
 *
 *  Created on: 9/08/2021
 *      Author: jdou557
 */


#include "Option.h"
#include <iostream>
#include "../lib/json.hpp"
using jsonObject = nlohmann::json;
using namespace std;

#ifndef OPTIONS_COLOUROPTION_H_
#define OPTIONS_COLOUROPTION_H_


class ColourOption : public Option {
public:
	ColourOption(string name, string section, string title);
	ColourOption(string name, string section, string title, string value);
	ColourOption(string name, string section, string title, string value, bool hide);
	virtual jsonObject toJSON() override;
	string getVal();
	void setVal(string val);

private:
	string value;
	vector<string> colours;
	void initCols();
};


#endif /* OPTIONS_COLOUROPTION_H_ */
