/*
 * PhylogenyAPI.cpp
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include "WasmAPI.h"
#include "../api/PhylogenyAPI.h"
#include "../api/AlignmentAPI.h"
#include "../api/EpiAPI.h"
#include "../api/OptionsAPI.h"
#include "../error/Error.h"
#include <emscripten.h>
#include <chrono>


using namespace std;
using namespace std::chrono;
using json = nlohmann::json;



// Interface between javascript and cpp for webassembly
extern "C" {


	/*
	 * Remove the tree
	 */
	void EMSCRIPTEN_KEEPALIVE removeTreeUpload(){
		PhylogenyAPI::cleanup();
		PhylogenyAPI::orderingIsDirty = true;
		EpiAPI::setEpiAnnotationsToDirty();


		//AlignmentAPI::initFiltering(OptionsAPI::getVariantSitesOnly(), false, nullptr);
		//OptionsAPI::prepareColourings();

		OptionsAPI::resetScroll();
		OptionsAPI::resetWindowSize();
		OptionsAPI::prepareTreeAnnotationOptions();
		
		if (AlignmentAPI::isMock()) AlignmentAPI::cleanup();
		
		WasmAPI::messageFromWasmToJS("");
	}


	void EMSCRIPTEN_KEEPALIVE uploadTree() {

		char* str = WasmAPI::getFromHeap();

		PhylogenyAPI::cleanup();

		cout << "uploading tree " << strlen(str) << endl;


		string contents(str);


		auto start = high_resolution_clock::now();

		//PhylogenyAPI::allTrees.push_back(THE_TREE);
		//PhylogenyAPI::treeNumber = 0;
		PhylogenyAPI::allTrees = Tree::parseTrees(contents);
		PhylogenyAPI::treeNumber = PhylogenyAPI::allTrees.size() - 1;
		PhylogenyAPI::THE_TREE = PhylogenyAPI::allTrees.at(PhylogenyAPI::treeNumber);


		
		
		PhylogenyAPI::orderingIsDirty = true;
		PhylogenyAPI::infectionCountIsDirty = true;

		if (Error::wasError()) {
			PhylogenyAPI::cleanup();
			return;
		}

		// If alignment has been uploaded, check it matches the tree
		PhylogenyAPI::prepareLabelling(AlignmentAPI::getAlignment());


		
		for (Tree* tree : PhylogenyAPI::allTrees){
			AlignmentAPI::annotateTaxa(tree);
		}
		
		// Epidemiological annotations
		EpiAPI::setEpiAnnotationsToDirty();
		EpiAPI::addAnnotationsToTree(PhylogenyAPI::THE_TREE);
		
		

		
		// Prepare tree annotation options
		OptionsAPI::prepareTreeAnnotationOptions();
		OptionsAPI::resetScroll();


		auto finish = high_resolution_clock::now();
		auto duration = duration_cast<seconds>(finish - start);




		cout << "Parsed " << PhylogenyAPI::allTrees.size() << " trees successfully (" << duration.count() << "s)" << endl;


		//cout << "tree " << PhylogenyAPI::THE_TREE->toNewick() << endl;

		json j;
		j["time"] = duration.count();
		WasmAPI::messageFromWasmToJS(j.dump(0));



	}



	/**
	 * Rotate the subtree at the specified internal node
	 * @param index
	 * @throws Exception
	 */
	void EMSCRIPTEN_KEEPALIVE flipSubree(int index) {

		if (PhylogenyAPI::THE_TREE != nullptr) {
			PhylogenyAPI::THE_TREE->flipSubtree(index);
			//PhylogenyAPI::orderingIsDirty = true;
			PhylogenyAPI::infectionCountIsDirty = true;
			EpiAPI::setEpiAccessionsToDirty();
			//AlignmentAPI::THE_ALIGNMENT->recalculateTaxonIndices();
			//PhylogenyAPI::prepareLabelling(AlignmentAPI::getAlignment());
			PhylogenyAPI::sortTaxaByTree(PhylogenyAPI::THE_TREE, AlignmentAPI::getAlignment());
		}
		WasmAPI::messageFromWasmToJS("{}");

	}



	/**
	 * Download the tree
	 */
	void EMSCRIPTEN_KEEPALIVE downloadTree() {
		
		jsonObject contents;
		if (PhylogenyAPI::THE_TREE != nullptr){
			contents["contents"] = PhylogenyAPI::THE_TREE->toNewick(AlignmentAPI::filtering);
		}
		WasmAPI::messageFromWasmToJS(contents.dump(0));
		
	}
	
	
	
	/**
	 * Go to next tree in posterior
	 */
	void EMSCRIPTEN_KEEPALIVE moveTree(int plus) {
		
		int origNumber = PhylogenyAPI::treeNumber;
		jsonObject contents;
		contents["treeNum"] = PhylogenyAPI::treeNumber;
		contents["changed"] = false;
		if (PhylogenyAPI::allTrees.size() == 0) {
			WasmAPI::messageFromWasmToJS(contents.dump(0));
			return;
		}
		
		PhylogenyAPI::treeNumber += plus;
		if (PhylogenyAPI::treeNumber < 0) PhylogenyAPI::treeNumber = 0;
		if (PhylogenyAPI::treeNumber >= PhylogenyAPI::allTrees.size()) PhylogenyAPI::treeNumber = PhylogenyAPI::allTrees.size()-1;
		
		PhylogenyAPI::THE_TREE = PhylogenyAPI::allTrees.at(PhylogenyAPI::treeNumber);
		PhylogenyAPI::orderingIsDirty = true;
		
		contents["treeNum"] = PhylogenyAPI::treeNumber;
		contents["changed"] = origNumber != PhylogenyAPI::treeNumber;
		cout << "Viewing tree number " << contents["treeNum"] << endl;
		WasmAPI::messageFromWasmToJS(contents.dump(0));
	}





}

