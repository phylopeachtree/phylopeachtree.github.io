/*
 * Timeline.h
 *
 *  Created on: 15/07/2021
 *      Author: jdou557
 */

#ifndef EPI_TIMELINE_H_
#define EPI_TIMELINE_H_

#include <time.h>
#include <iostream>
#include <map>
#include "Epidemiology.h"

using namespace std;



class Node;
class Timeline {
public:

	const static vector<string> dateFormats;
	const static double AXIS_MARGIN;
	const static int MAX_NDATES;


	Timeline(Tree* tree, Epidemiology* epidemiology, string sampleDateVariable, string dateFormat);

	bool isReady();
	void setSampleDateVariable(string sampleDateVariable, string dateFormat);
	void cleanup();
	double getSampleTimeOfCase(int caseNum);
	double getSampleHeightOfCase(int caseNum);
	tm getSampleDateOfCase(Node* node);
	double getSampleHeight(Node* node);
	jsonObject getTimelineGraphics(Node* subtree, Scaling* scaling, double axisFontSize, string symptomDateVar,
			int infectiousDaysBefore, int infectiousDaysAfter, string isolationDateVar);
	void prepareNodeSampleHeights(Node* subtree, string symptomDateVar, int infectiousDaysBefore, int infectiousDaysAfter);



	tm getLatestDate();
	tm getEarliestDate();
	//double getLatestTime();
	//double getEarliestTime();

	static string getDefaultDateFormat();
	static string getCanonicalDateFormat(string dateFormat);


private:
	Epidemiology* epidemiology;
	string sampleDateVariable;
	string dateFormat;
	string dateFormatCanonical;
	vector<tm> caseDates;
	Node* sampleSubtree;

	tm latestDate;
	tm earliestDate;
	double latestTime;
	double earliestTime;

	double earliestTreeLeafHeight;

	double meanTipHeight_cases;
	double meanTipHeight_tree;


	void resetNodeSampleTimes(Node* node);



};

#endif /* EPI_TIMELINE_H_ */
