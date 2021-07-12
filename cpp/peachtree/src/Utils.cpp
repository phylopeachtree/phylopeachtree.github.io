/*
 * Utils.cpp
 *
 *  Created on: 2/07/2021
 *      Author: jdou557
 */

#include "Utils.h"


#include <iostream>
#include <cstring>
#include <cmath>
#include <cstdlib>

using namespace std;


const double Utils::INFTY = INFINITY;

/**
 * Split a string by a delimiter
 */
vector<string> Utils::split(string s, string delimiter) {
	return Utils::split(s, delimiter, -1);
}




/*
 * Is the string a number?
 */
bool Utils::is_number(const std::string& s) {
    char* end = nullptr;
    double val = strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

/**
 * Split a string by a delimiter, but only the 1st 'limit' occurrences (or infinity if limit<=0).
 */
vector<string> Utils::split(string s, string delimiter, int limit) {


	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
		token = s.substr (pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back (token);

		if (limit > 0 && res.size() == limit-1){
			break;
		}

		//cout << token << endl;
	}

	res.push_back(s.substr (pos_start));
	return res;

}


/*
 * String to upper case
 */
string Utils::toUpper(string str){

	int i=0;
	char cstr[str.size() + 1];
	strcpy(cstr, str.c_str());

	while (cstr[i]) {
		cstr[i] = toupper(cstr[i]);
		i++;
	}

	string result(cstr);
	return result;

}


/*
 * String to lower case
 */
string Utils::toLower(string str){

	int i=0;
	char cstr[str.size() + 1];
	strcpy(cstr, str.c_str());

	while (cstr[i]) {
		cstr[i] = tolower(cstr[i]);
		i++;
	}

	string result(cstr);
	return result;

}


// String trim from start (in place)
void Utils::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// String trim from end (in place)
void Utils::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Trim from both ends (in place)
void Utils::trim(string& s) {
    ltrim(s);
    rtrim(s);
}






