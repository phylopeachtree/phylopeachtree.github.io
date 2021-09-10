/*
 * Utils.h
 *
 *  Created on: 2/07/2021
 *      Author: jdou557
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include "lib/json.hpp"

using jsonObject = nlohmann::json;


using namespace std;

#ifndef UTILS_H_
#define UTILS_H_




class Utils {

public:

	static const double INFTY;


	static vector<string> split(string str, string delimiter);
	static vector<string> split(string str, string delimiter, int limit);
	static string toUpper(string str);
	static string toLower(string str);
	static void trim(string& s);
	static bool is_number(const std::string& s);
	static bool parseDate(string date, string format, tm& time);
	static double getTimeFromDate(tm& time);
	//static void sortDates(vector<tm>& times);
	static bool compareDates(const tm &d1, const tm &d2);
	static tm addYears(tm t1, double years);
	static tm addDays(tm t1, double days);
	static string formatDate(tm date);
	static void getNiceTimes(tm lastDate, double timeSpan, int maxNTimes, vector<double>& heights, vector<string>& labels);
	static int getRed(string hexCol);
	static int getGreen(string hexCol);
	static int getBlue(string hexCol);
	static string getHexCode(int red, int green, int blue);
	static double roundToSF(double N, int n);
	static double log10(double val);
	static string toSVG(double width, double height, jsonObject json);
	
	static string openFile(string fileName);
	

	static const vector<string> month_names;
	

private:

	static void ltrim(std::string& s);
	static void rtrim(std::string& s);


};

#endif /* UTILS_H_ */
