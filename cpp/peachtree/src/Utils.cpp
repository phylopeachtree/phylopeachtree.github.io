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
const vector<string> Utils::month_names({"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"});

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


/*
 * Attempt to parse a date/time string into &tm using the format eg. "%d/%m/%Y"
 */
bool Utils::parseDate(string date, string format, tm& time){
	if (strptime(date.c_str(), format.c_str(), &time)) return true;
	return false;
}


/*
 * Convert a datetime into units of year, since the year 0BC
 */
double Utils::getTimeFromDate(tm& tm){
	int day = tm.tm_mday;
	int month = tm.tm_mon + 1;
	int year = tm.tm_year + 1900;
	return year + month/12.0 + day/365.0;
}



/*
 * Sort a vector of dates
 */
//void Utils::sortDates(vector<tm>& times){
	//sort(times.begin(), times.end(), Utils::compareDates);
//}




/*
 * Compare 2 dates (d1 < d2) ?
*/
bool Utils::compareDates(const tm &d1, const tm &d2){

	 // All cases when true should be returned
	if (d1.tm_year < d2.tm_year)
		return true;
	if (d1.tm_year == d2.tm_year && d1.tm_mon < d2.tm_mon)
		return true;
	if (d1.tm_year == d2.tm_year && d1.tm_mon == d2.tm_mon && d1.tm_mday < d2.tm_mday)
		return true;

	// If none of the above cases satisfy, return false
	return false;
}




/*
 * Find up to 'maxNTimes' times in this interval, so that they are evenly spaced and are nice dates eg. 1st of every month
 */
void Utils::getNiceTimes(tm lastDate, double timeSpan, int maxNTimes, vector<double>& heights, vector<string>& labels){



	timeSpan = std::abs(timeSpan);

	heights.clear();
	labels.clear();

	tm firstDate = Utils::addYears(lastDate, -timeSpan);
	double timeAtLastTip = Utils::getTimeFromDate(lastDate);
	int currentYear = 0;


	double nMonthsInTimespan = timeSpan*12;
	bool countDays = nMonthsInTimespan < 6; // Less than 6 months
	bool countMonthsNotDays = !countDays && timeSpan < maxNTimes; // Between 6 months and 2 years

	// How long is a time interval?
	double deltaTime = timeSpan / maxNTimes;
	if (countMonthsNotDays){

		// Every 1, 2, 3, 4, 5, or 6 months
		vector<int> niceGaps({1,2,3,4,5,6});
		for (int g : niceGaps){
			if (nMonthsInTimespan / g <= maxNTimes){
				deltaTime = g/12.0;
				//cout << "nice gaps " << g << " (" << maxNTimes << ")" << endl;
				break;
			}
		}


	}



	// Print day-month-year for the 1st date in each year, and day-month afterwards
	for (int t = 1; t <= maxNTimes; t++){

		tm date = Utils::addYears(firstDate, deltaTime*t);



		// Round the days?
		if (countDays){

			// If theres more than maxNTimes/2 months, round it back to the 1st or the 15th of the month
			if (timeSpan*12 > maxNTimes/2) {
				if (date.tm_mday < 15) date.tm_mday = 1;
				else date.tm_mday = 15;
			}


			// Otherwise, try round it back to the 1/10/21 of the month
			else if (timeSpan*12 > maxNTimes/3) {
				if (date.tm_mday < 11) date.tm_mday = 1;
				else if (date.tm_mday < 21) date.tm_mday = 11;
				else date.tm_mday = 21;
			}

		}


		// Round down to the 1st of each month
		else if (countMonthsNotDays){
			date.tm_mday = 1;
		}


		// Round down to the 1st of each year
		else{
			date.tm_mday = 1;
			date.tm_mon = 0;
		}


		double height = timeAtLastTip - Utils::getTimeFromDate(date);

		int day = date.tm_mday;
		string mon = Utils::month_names.at(date.tm_mon);
		int year = date.tm_year + 1900;

		string str = "";


		// 1. If time span is less than 6 months, then print days and months (and years only when they change)
		if (countDays) {
			str = to_string(day) + " " + mon;
		}


		// 2. If the time span is between 6 and 24 months, print months (and years only when they change)
		else if (countMonthsNotDays){
			str = mon;
		}

		// 3. Print years only
		else{
			str = year;
		}


		// Print year only on year change
		if ((countDays || countMonthsNotDays) && year != currentYear){
			str = str + " " + to_string(year);
			currentYear = year;
		}


		// Check for duplicates
		if (std::count(labels.begin(), labels.end(), str) == 0){
			heights.push_back(height);
			labels.push_back(str);
		}



	}





}


/*
 * Add years to a date
 */
tm Utils::addYears(tm t1, double years) {
    time_t epoch = mktime(&t1);
    epoch += (60*60*24*365 * years);
    return *localtime(&epoch);
}



/*
 * Add days to date
 */
tm Utils::addDays(tm t1, double days){
	time_t epoch = mktime(&t1);
	epoch += (60*60*24 * days);
	return *localtime(&epoch);
}


/*
 * Format date as dd mon yyyy
 */
string Utils::formatDate(tm date){

	int day = date.tm_mday;
	int mon = date.tm_mon;
	int year = date.tm_year + 1900;


	return to_string(day) + " " + Utils::month_names.at(mon) + " " + to_string(year);

}










