/*
 * NumericalOption.h
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include "Option.h"
#include <iostream>
#include "../lib/json.hpp"
using jsonObject = nlohmann::json;
using namespace std;


#ifndef OPTIONS_NUMERICALOPTION_H_
#define OPTIONS_NUMERICALOPTION_H_

class NumericalOption : public Option {
public:
	NumericalOption(string name, string section, string title, double val, double min, double max, double stepSize);
	NumericalOption(string name, string section, string title, double val, double min, double max, double stepSize, bool hidden);
	virtual jsonObject toJSON() override;
	double getVal();
	void setVal(double val);

private:
	double value;
	double min;
	double max;
	double stepSize;
};

#endif /* OPTIONS_NUMERICALOPTION_H_ */
