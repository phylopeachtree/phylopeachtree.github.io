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
	static bool parseDate(string date, string format, tm& time);
	static double getTimeFromDate(tm& time);
	//static void sortDates(vector<tm>& times);
	static bool compareDates(const tm &d1, const tm &d2);
	static tm addYears(tm t1, double years);
	static string formatDate(tm date);
	static void getNiceTimes(tm lastDate, double timeSpan, int maxNTimes, vector<double>& heights, vector<string>& labels);

	static const vector<string> month_names;

private:

	static void ltrim(std::string& s);
	static void rtrim(std::string& s);


};

#endif /* UTILS_H_ */
