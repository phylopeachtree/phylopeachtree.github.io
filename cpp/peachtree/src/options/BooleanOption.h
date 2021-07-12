/*
 * BooleanOption.h
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */


#include "Option.h"
#include <iostream>
#include "../lib/json.hpp"
using jsonObject = nlohmann::json;
using namespace std;

#ifndef OPTIONS_BOOLEANOPTION_H_
#define OPTIONS_BOOLEANOPTION_H_

class BooleanOption : public Option {
public:
	BooleanOption(string name, string section, string title, bool value);
	BooleanOption(string name, string section, string title, bool value, bool hide);
	virtual jsonObject toJSON() override;
	bool getVal();
	void setVal(bool val);

private:
	bool value;
};

#endif /* OPTIONS_BOOLEANOPTION_H_ */
