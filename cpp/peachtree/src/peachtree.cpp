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
	cout << "hello there" << endl; // prints !!!Hello World!!!




	//testAlignment(1);

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









