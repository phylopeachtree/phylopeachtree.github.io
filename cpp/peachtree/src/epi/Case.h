/*
 * Case.h
 *
 *  Created on: 14/07/2021
 *      Author: jdou557
 */

#ifndef EPI_CASE_H_
#define EPI_CASE_H_


#include <iostream>
#include <map>

#include "../lib/json.hpp"
using namespace std;
using jsonObject = nlohmann::json;


class Case {
public:
	Case(string accession, map<string, string> annotations);
	string getValue(string var);
	string getAccession();
	vector<string> getVariables();


private:
	string accession;
	map<string, string> annotations;
};

#endif /* EPI_CASE_H_ */
