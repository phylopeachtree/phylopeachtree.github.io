//============================================================================
// Name        : peachtree.cpp
// Author      : Jordan Douglas
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "aln/Alignment.h"
#include "Utils.h"

#include "lib/json.hpp"
using jsonObject = nlohmann::json;

//#include <json.hpp>


using namespace std;


void test(string& str){
	str = "bye";
}


int main() {

	cout << "hello there" << endl;

	string col = "#baff00";
	int red = Utils::getRed(col);
	int green = Utils::getGreen(col);
	int blue = Utils::getBlue(col);

	cout << "rgb " << red << "/" << green << "/" << blue << endl;


	string rgb = Utils::getHexCode(red, green, blue);
	cout << "rgb " << rgb << endl;

	/*
	tm t1;
	std::fill((char*)&t1, (char*)&t1 + sizeof(t1), 0);
	t1.tm_year = 2021;
	t1.tm_mon = 4;
	t1.tm_mday = 1;



	auto result = Utils::addYears(t1, -0.5);

	std::cout << result.tm_year << '/' << result.tm_mon << '/' << result.tm_mday << std::endl;


	vector<double> heights;
	vector<string> labels;
	Utils::getNiceTimes(t1, 0.9, 5, heights, labels);
	for (int i = 0; i < heights.size(); i ++){
		cout << heights.at(i) << " / " << labels.at(i) << endl;
	}

	cout << "-------------" << endl;
	Utils::getNiceTimes(t1, 0.2, 5, heights, labels);
	for (int i = 0; i < heights.size(); i ++){
		cout << heights.at(i) << " / " << labels.at(i) << endl;
	}



	cout << "-------------" << endl;
	Utils::getNiceTimes(t1, 1.5, 5, heights, labels);
	for (int i = 0; i < heights.size(); i ++){
		cout << heights.at(i) << " / " << labels.at(i) << endl;
	}
	*/

	/*


	cout << tm1.tm_year << "/" << tm1.tm_mon << "/" << tm1.tm_mday << endl;

	tm out = Utils::addTimeToDate(tm1, -0.1);
	cout << out.tm_year << "/" << out.tm_mon << "/" << out.tm_mday << endl;

	tm out2 = Utils::addTimeToDate(tm1, 1.5);
	cout << out2.tm_year << "/" << out2.tm_mon << "/" << out2.tm_mday << endl;
	*/


	/*
	vector<int>* vec1 = new vector<int>();
	vec1->push_back(2);
	test(vec1);

	for (int i = 0; i < vec1->size(); i ++) {
		cout << vec1->at(i) << endl;
	}
*/

	/*
	jsonObject arr = json::array();
	arr.push_back("X");
	jsonObject arr2;// = json::array();
	arr2.push_back("Y");
	arr2.push_back("Z");
	arr.insert(arr.end(), arr2.begin(), arr2.end());
	cout << arr.dump(2) << endl;
	*/

	string str = ">aaa\nacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgt";
	//Alignment* aln;

	for (int i = 0; i < 10; i ++){
		Alignment* aln = new Alignment(str);
		//aln->cleanup();
	}


	return 0;
}









