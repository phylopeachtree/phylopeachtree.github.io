/*
 * Option.h
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include <iostream>
#include "../lib/json.hpp"

using jsonObject = nlohmann::json;
using namespace std;


#ifndef OPTIONS_OPTION_H_
#define OPTIONS_OPTION_H_

class Option {
public:
	virtual ~Option();
	virtual jsonObject toJSON();
	string getName() ;
	string getSection();
	string getTitle();
	void hide();
	void show();
	jsonObject modifyJSON(jsonObject json);

protected:
	string name; // Name of variable
	string section; // Category it belongs to
	string title; // Description
	bool hidden; // Hide it?

};

#endif /* OPTIONS_OPTION_H_ */
