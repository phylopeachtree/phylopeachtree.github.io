/*
 * EpiAPI.cpp
 *
 *  Created on: 14/07/2021
 *      Author: jdou557
 */


#include <chrono>
#include <emscripten.h>

#include "../api/EpiAPI.h"
#include "../api/PhylogenyAPI.h"
#include "../api/AlignmentAPI.h"
#include "../api/OptionsAPI.h"
#include "../error/Error.h"

#include "WasmAPI.h"

using namespace std::chrono;


// Interface between javascript and cpp for webassembly
extern "C" {



	/*
	 * Remove the epi data
	 */
	void EMSCRIPTEN_KEEPALIVE removeEpiUpload(){
		EpiAPI::cleanup();
		OptionsAPI::resetScroll();
		OptionsAPI::resetWindowSize(true, true);
		OptionsAPI::prepareEpiAnnotations();
		OptionsAPI::prepareTreeAnnotationOptions();
		WasmAPI::messageFromWasmToJS("");
	}


	void EMSCRIPTEN_KEEPALIVE uploadEpi() {

		cout << "Uploading epidemiological information" << endl;

		char* str = WasmAPI::getFromHeap();
		string contents(str);


		auto start = high_resolution_clock::now();


		EpiAPI::cleanup();

		EpiAPI::EPIDEMIOLOGY = new Epidemiology();
		EpiAPI::EPIDEMIOLOGY->parseFile(contents);
		EpiAPI::setEpiAccessionsToDirty();
		EpiAPI::validateAccessions(AlignmentAPI::getAlignment());
		

		
		// Taxon annotations
		AlignmentAPI::annotateTaxa(EpiAPI::EPIDEMIOLOGY);
		
		
		// Annotate tree
		EpiAPI::setEpiAnnotationsToDirty();
		EpiAPI::addAnnotationsToTree(PhylogenyAPI::THE_TREE);
		

		OptionsAPI::prepareEpiAnnotations();
		OptionsAPI::prepareTreeAnnotationOptions();

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
