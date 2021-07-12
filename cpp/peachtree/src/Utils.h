/*
 * Utils.h
 *
 *  Created on: 2/07/2021
 *      Author: jdou557
 */

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

#ifndef UTILS_H_
#define UTILS_H_




class Utils {
public:

	static vector<string> split(string str, string delimiter);
	static vector<string> split(string str, string delimiter, int limit);
	static string toUpper(string str);
	static string toLower(string str);
	static const double INFTY;
	static void trim(string& s);
	static bool is_number(const std::string& s);

private:

	static void ltrim(std::string& s);
	static void rtrim(std::string& s);

};

#endif /* UTILS_H_ */
