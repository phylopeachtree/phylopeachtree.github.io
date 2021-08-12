/*
 * EpiAPI.h
 *
 *  Created on: 14/07/2021
 *      Author: jdou557
 */

#ifndef WASM_EPIAPI_H_
#define WASM_EPIAPI_H_

#include <iostream>
#include "../epi/Epidemiology.h"
#include "../epi/Timeline.h"
#include "../aln/Alignment.h"
#include "../aln/Filtering.h"
#include "../phy/Tree.h"
#include "../lib/json.hpp"
using namespace std;
using jsonObject = nlohmann::json;


class EpiAPI {
public:
	static void setEpiAccessionsToDirty();
	static void setEpiAnnotationsToDirty();
	static void validateAccessions(Alignment* alignment);
	static void addAnnotationsToTree(Tree* tree);
	static vector<string> getAllAnnotations();
	static void cleanup();
	static void prepareTimeline(Tree* tree, string sampleDateVariable, string dateFormat);
	static jsonObject getTimelineGraphics(Node* subtree, Scaling* scaling, double axisFontSize, string symptomDateVar,
			int infectiousDaysBefore, int infectiousDaysAfter, string isolationDateVar, double branchWidth);
	static Timeline* getTimeline();
	static void prepareNodeSampleHeights(Node* subtree, string symptomDateVar, int infectiousDaysBefore, int infectiousDaysAfter);

	static Epidemiology* EPIDEMIOLOGY;
	static bool epiAccessionsAreDirty;
	static bool epiAnnotationsAreDirty;
	static Timeline* timeline;

};

#endif /* WASM_EPIAPI_H_ */
