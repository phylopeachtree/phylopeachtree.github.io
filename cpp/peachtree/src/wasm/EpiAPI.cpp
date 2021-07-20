/*
 * EpiAPI.cpp
 *
 *  Created on: 14/07/2021
 *      Author: jdou557
 */


#include <chrono>
#include <emscripten.h>

#include "EpiAPI.h"
#include "PhylogenyAPI.h"
#include "AlignmentAPI.h"
#include "OptionsAPI.h"
#include "../error/Error.h"

using namespace std::chrono;


Epidemiology* EpiAPI::EPIDEMIOLOGY = nullptr;
Timeline* EpiAPI::timeline = nullptr;
bool EpiAPI::epiAccessionsAreDirty = false;
bool EpiAPI::epiAnnotationsAreDirty = false;



/*
 * Get timeline graphics
 */
jsonObject EpiAPI::getTimelineGraphics(Node* subtree, Scaling* scaling, double axisFontSize, string symptomDateVar, int infectiousDaysBefore, int infectiousDaysAfter){
	if (EpiAPI::timeline == nullptr || !EpiAPI::timeline->isReady()) {
		jsonObject arr = json::array();
		return arr;
	}
	return EpiAPI::timeline->getTimelineGraphics(subtree, scaling, axisFontSize, symptomDateVar, infectiousDaysBefore, infectiousDaysAfter);
}




/*
 * Reset sample heights if the subtree has changed
 */
void EpiAPI::prepareNodeSampleHeights(Node* subtree){
	if (EpiAPI::timeline == nullptr) return;
	EpiAPI::timeline->prepareNodeSampleHeights(subtree);
}


/*
 * Prepare timeline
 */
void EpiAPI::prepareTimeline(Tree* tree, string sampleDateVariable, string dateFormat){

	if (EpiAPI::EPIDEMIOLOGY == nullptr) return;
	if (timeline == nullptr || epiAccessionsAreDirty){
		timeline = new Timeline(tree, EpiAPI::EPIDEMIOLOGY, sampleDateVariable, dateFormat);
	}else{
		timeline->setSampleDateVariable(sampleDateVariable, dateFormat);
	}

}

/*
 * Indicate that the accessions need to be revalidated
 */
void EpiAPI::setEpiAccessionsToDirty(){
	epiAccessionsAreDirty = true;
}

/*
 * Return the timeline
 */
Timeline* EpiAPI::getTimeline(){
	return EpiAPI::timeline;
}

/*
 * Indicate that the annotations need to be added to the tree
 */
void EpiAPI::setEpiAnnotationsToDirty(){
	epiAnnotationsAreDirty = true;
}


/*
 * Validate the accessions in the epidemiology with those in the alignment
 */
void EpiAPI::validateAccessions(Alignment* alignment){
	if (!epiAccessionsAreDirty) return;
	if (alignment == nullptr) return;
	if (EPIDEMIOLOGY == nullptr) return;
	EPIDEMIOLOGY->validateAccessions(alignment);
	epiAccessionsAreDirty = false;
}


/*
 * Add annotations to the tree
 */
void EpiAPI::addAnnotationsToTree(Tree* tree){
	if (!epiAnnotationsAreDirty) return;
	if (tree == nullptr) return;
	if (EPIDEMIOLOGY == nullptr) return;
	EPIDEMIOLOGY->addAnnotationsToTree(tree);
	epiAnnotationsAreDirty = false;
}



/*
 * List of annotations
 */
vector<string> EpiAPI::getAllAnnotations(){
	if (EPIDEMIOLOGY == nullptr) {
		vector<string> x;
		return x;
	}
	return EPIDEMIOLOGY->getAnnotations();


}


void EpiAPI::cleanup(){
	if (EPIDEMIOLOGY != nullptr){
		EPIDEMIOLOGY->cleanup();
		delete EPIDEMIOLOGY;
		EPIDEMIOLOGY = nullptr;
	}
	if (timeline != nullptr){
		timeline->cleanup();
		delete timeline;
		timeline = nullptr;
	}
}


// Interface between javascript and cpp for webassembly
extern "C" {

	void EMSCRIPTEN_KEEPALIVE uploadEpi() {

		cout << "Uploading epidemiological information" << endl;

		char* str = WasmAPI::getFromHeap();
		string contents(str);


		auto start = high_resolution_clock::now();


		EpiAPI::cleanup();

		EpiAPI::EPIDEMIOLOGY = new Epidemiology();
		EpiAPI::EPIDEMIOLOGY->parseFile(contents);
		EpiAPI::setEpiAccessionsToDirty();
		EpiAPI::setEpiAnnotationsToDirty();
		EpiAPI::validateAccessions(AlignmentAPI::getAlignment());
		EpiAPI::addAnnotationsToTree(PhylogenyAPI::getTree());

		OptionsAPI::prepareEpiAnnotations();

		auto finish = high_resolution_clock::now();
		auto duration = duration_cast<seconds>(finish - start);


		if (Error::wasError()) {
			EpiAPI::cleanup();
			return;
		}

		cout << "Parsed successfully (" << duration.count() << "s)" << endl;

		json j;
		j["time"] = duration.count();
		WasmAPI::messageFromWasmToJS(j.dump(0));


	}

}
