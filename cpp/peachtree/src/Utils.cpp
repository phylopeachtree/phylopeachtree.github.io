/*
 * Utils.cpp
 *
 *  Created on: 2/07/2021
 *      Author: jdou557
 */

#include "Utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
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
*	Log base 10
*/
double Utils::log10(double val){
	return std::log(val) / std::log(10);
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


	double nDaysInTimespan = timeSpan*365;
	double nMonthsInTimespan = timeSpan*12;
	bool countDays = nMonthsInTimespan < 6; // Less than 6 months
	bool countMonthsNotDays = !countDays && timeSpan < maxNTimes; // Between 6 months and 2 years

	// How long is a time interval?
	double deltaTime = timeSpan / maxNTimes;

	// Every 1,2,3,4,5,10 days
	if (countDays){

		vector<int> niceGaps({1,2,3,4,5,10});
		for (int g : niceGaps){
			if (nDaysInTimespan / g <= maxNTimes){
				deltaTime = g/365.0;
				//cout << "nice gaps " << g << " (" << maxNTimes << ")" << endl;
				break;
			}
		}

	}


	// Every 1, 2, 3, 4, 5, or 6 months
	if (countMonthsNotDays){


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

			// If theres more than 3 months, round it back to the 1st or the 15th of the month
			if (nMonthsInTimespan > 3) {
				if (date.tm_mday < 15) date.tm_mday = 1;
				else date.tm_mday = 15;
			}


			// If more than 1 month, round it to 1, 11, 21
			else if (nMonthsInTimespan > 1) {
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
			str = to_string(year);
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



/**
 * Get red (0-255) from hex code
 */
int Utils::getRed(string hexCol){

	string substr = hexCol.substr(1,2);
	unsigned int x;
	std::stringstream ss;
	ss << std::hex << substr;
	ss >> x;
	return x;

}

/**
 * Get green (0-255) from hex code
 */
int Utils::getGreen(string hexCol){

	string substr = hexCol.substr(3,2);
	unsigned int x;
	std::stringstream ss;
	ss << std::hex << substr;
	ss >> x;
	return x;

}


/**
 * Get blue (0-255) from hex code
 */
int Utils::getBlue(string hexCol){

	string substr = hexCol.substr(5,2);
	unsigned int x;
	std::stringstream ss;
	ss << std::hex << substr;
	ss >> x;
	return x;

}


/*
 * Get a hex code
 */
string Utils::getHexCode(int red, int green, int blue){

	// Red
	std::stringstream red_stream;
	red_stream << std::hex << red;
	std::string red_str( red_stream.str() );
	if (red_str.size() == 1) red_str = "0" + red_str;

	// Green
	std::stringstream green_stream;
	green_stream << std::hex << green;
	std::string green_str( green_stream.str() );
	if (green_str.size() == 1) green_str = "0" + green_str;

	// Blue
	std::stringstream blue_stream;
	blue_stream << std::hex << blue;
	std::string blue_str( blue_stream.str() );
	if (blue_str.size() == 1) blue_str = "0" + blue_str;


	return "#" + red_str + green_str + blue_str;


}


/*
 * Round to sf
 * https://www.geeksforgeeks.org/round-off-number-given-number-significant-digits/
 */
double Utils::roundToSF(double N, int n){


	int h;
	double l, a, b, c, d, e, i, j, m, f, g;
	b = N;
	c = floor(N);

	// Counting the no. of digits to the left of decimal point
	// in the given no.
	for (i = 0; b >= 1; ++i)
		b = b / 10;

	d = n - i;
	b = N;
	b = b * pow(10, d);
	e = b + 0.5;
	if ((float)e == (float)ceil(b)) {
		f = (ceil(b));
		h = f - 2;
		if (h % 2 != 0) {
			e = e - 1;
		}
	}
	j = floor(e);
	m = pow(10, d);
	j = j / m;

	return j;

}



/*
 * Return file contents
 */
string Utils::openFile(string fileName){
	std::ifstream t(fileName);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}



/*
 * Convert json to a vector of svg elements
 * Each vector refers to a single group
 */
vector<vector<string>> Utils::toSVGVectors(jsonObject json){
	
	
	vector<string> btmG;
	vector<string> midG;
	vector<string> topG;
	
	size_t pos;
	for (int i = 0; i < json.size(); i ++){
		
		//cout << i << "/" << json.size() << endl;
		
		jsonObject obj = json.at(i);
		string eleAType = to_string(obj["ele"]);
		
		while ((pos = eleAType.find("\"")) != std::string::npos) eleAType = eleAType.replace(pos, 1, "");
		
		stringstream ele;
		stringstream value_stream;
		if (obj["value"] != nullptr) {
			string value = to_string(obj["value"]);
			while ((pos = value.find("\"")) != std::string::npos) value = value.replace(pos, 1, "");
			value_stream << value;
		}
		
		int layer = 1;
		if (obj["layer"] != nullptr) layer = obj["layer"];
		ele << "<" << eleAType << " ";
		
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			//std::cout << it.key() << " | " << it.value() << "\n";
			string key = it.key();
			string val = to_string(it.value());
			
			if (key == "ele" || key == "layer" || key == "value") continue;
			
			// Remove ""
			while ((pos = val.find("\"")) != std::string::npos) val = val.replace(pos, 1, "");
			
			if (key == "title"){
				
				
				vector<string> lines = Utils::split(val, "\\n");
				
				value_stream << "<title>" << endl;
				for (string line : lines) value_stream << line << endl;

				value_stream << "</title>";
				
				
			}else{
			
				// Replace all _ with -
				while ((pos = key.find("_")) != std::string::npos) key = key.replace(pos, 1, "-");
				ele << key << "=\"" << val << "\" ";
			}
					
			
		}
		 
		//string valStr = value.str();
		string value = value_stream.str();
		if (!value.empty()) ele << ">" << value << "</" << eleAType << ">";
		else ele << "/>";
		
		
		
		string eleStr = ele.str();
		//while ((pos = eleStr.find("\\\"")) != std::string::npos) eleStr = eleStr.replace(pos, 1, "'");

		//cout << eleStr << endl;
		if (layer == 0) btmG.push_back(eleStr);
		else if (layer == 1) midG.push_back(eleStr);
		else topG.push_back(eleStr);
        
		
	}
	
	
	
	// Build 2D vector
	vector<vector<string>> allGroups;
	allGroups.push_back(btmG);
	allGroups.push_back(midG);
	allGroups.push_back(topG);
	return allGroups;
	
}





/*
 * Convert json to a string of svg elements
 */
string Utils::toSVG(jsonObject json){
	
	
	
	stringstream svg;
	

	// Build strings
	vector<vector<string>> allGroups = Utils::toSVGVectors(json);
	
	for (int g = 0; g < allGroups.size(); g ++){
		
		svg << "<g>";
		vector<string> groupElements = allGroups.at(g);
		for (int i = 0; i < groupElements.size(); i++){
			svg << groupElements.at(i) << endl;
		}
		svg << "</g>" << endl;
		
	}
	
	
	return svg.str();
	
}



string Utils::toSVG(double width, double height, jsonObject json){
	
	
	//<?xml version="1.0" standalone="no"?>
	//<svg id="downloadSVG" style="font-family: &quot;Courier New&quot;; dominant-baseline: middle; height: 9464.53px; width: 1487px;" xmlns="http://www.w3.org/2000/svg">
	
	
	
	//cout << json.dump(0) << endl;
	
	stringstream svg;
	
	svg << "<?xml version='1.0' standalone='no'?>" << endl;
	svg << "<svg style='font-family: \"Courier New\"; dominant-baseline: middle; height: " << height << "px; width: " << width << "px;' xmlns='http://www.w3.org/2000/svg' viewBox='0 0 " << width << " " << height << "'>" << endl;
	
	// css
	svg << "<defs>" << endl;
    svg << "<style type='text/css'><![CDATA[" << endl;
			svg << ".taxon.selected  {" << endl;
			svg << "fill:red;" << endl;
			svg << "font-weight:bold;" << endl;
			svg << "}" << endl;
    svg << "]]></style>" << endl;
	svg << "</defs>" << endl;
	
	svg << Utils::toSVG(json);
	
	svg << "</svg>" << endl;
	
	return svg.str();

	
}




