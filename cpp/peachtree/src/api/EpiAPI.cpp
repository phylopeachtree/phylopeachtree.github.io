/*
 * EpiAPI.cpp
 *
 *  Created on: 14/07/2021
 *      Author: jdou557
 */


#include <chrono>

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



void EpiAPI::setEpi(Epidemiology* epi){
	EPIDEMIOLOGY = epi;
	epiAccessionsAreDirty = true;
	epiAnnotationsAreDirty = true;
}


/*
 * Get timeline graphics
 */
jsonObject EpiAPI::getTimelineGraphics(Node* subtree, Scaling* scaling, double axisFontSize, string symptomDateVar,
				int infectiousDaysBefore, int infectiousDaysAfter, string isolationDateVar, double branchWidth){
	if (EpiAPI::timeline == nullptr || !EpiAPI::timeline->isReady()) {
		jsonObject arr = json::array();
		return arr;
	}
	return EpiAPI::timeline->getTimelineGraphics(subtree, scaling, axisFontSize, symptomDateVar, infectiousDaysBefore, infectiousDaysAfter,
			isolationDateVar, branchWidth);
}


bool EpiAPI::isReady(){
	return EpiAPI::EPIDEMIOLOGY != nullptr;
}

/*
 * Reset sample heights if the subtree has changed
 */
void EpiAPI::prepareNodeSampleHeights(Node* subtree, string symptomDateVar, int infectiousDaysBefore, int infectiousDaysAfter){
	if (EpiAPI::timeline == nullptr) return;
	EpiAPI::timeline->prepareNodeSampleHeights(subtree, symptomDateVar, infectiousDaysBefore, infectiousDaysAfter);
}


/*
 * Prepare timeline
 */
void EpiAPI::prepareTimeline(Tree* tree, string sampleDateVariable, string dateFormat){

	if (EpiAPI::EPIDEMIOLOGY == nullptr) return;
	if (timeline == nullptr || epiAccessionsAreDirty){
		if (timeline != nullptr) {
			timeline->cleanup();
			delete timeline;
		}
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
		EPIDEMIOLOGY->clearAnnotationsFromTree(PhylogenyAPI::THE_TREE);
		delete EPIDEMIOLOGY;
		EPIDEMIOLOGY = nullptr;
	}
	if (timeline != nullptr){
		timeline->cleanup();
		delete timeline;
		timeline = nullptr;
	}
}

