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


Epidemiology* EpiAPI::EPIDEMIOLOGY;
bool EpiAPI::epiAccessionsAreDirty;
bool EpiAPI::epiAnnotationsAreDirty;




/*
 * Indicate that the accessions need to be revalidated
 */
void EpiAPI::setEpiAccessionsToDirty(){
	epiAccessionsAreDirty = true;
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
	EPIDEMIOLOGY->cleanup();
	delete EPIDEMIOLOGY;
	EPIDEMIOLOGY = nullptr;
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
