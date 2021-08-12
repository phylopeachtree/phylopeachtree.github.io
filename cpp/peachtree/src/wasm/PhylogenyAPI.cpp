/*
 * PhylogenyAPI.cpp
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include "PhylogenyAPI.h"
#include "AlignmentAPI.h"
#include "EpiAPI.h"
#include "OptionsAPI.h"
#include "../error/Error.h"
#include <emscripten.h>
#include <chrono>


using namespace std;
using namespace std::chrono;
using json = nlohmann::json;


Tree* PhylogenyAPI::THE_TREE;
bool PhylogenyAPI::orderingIsDirty;




Tree* PhylogenyAPI::getTree(){
	return PhylogenyAPI::THE_TREE;
}


void PhylogenyAPI::setOrderingToDirty(){
	PhylogenyAPI::orderingIsDirty = true;
}


/*
 * Return a list of annotations
 */
vector<string> PhylogenyAPI::getAllAnnotations(){
	vector<string> list;
	if (PhylogenyAPI::THE_TREE == nullptr) return list;
	return PhylogenyAPI::THE_TREE->getAllAnnotations();
}



/*
 * Sort the taxa in the alignment by the tree
 */
void PhylogenyAPI::sortTaxaByTree(Tree* tree, Alignment* alignment){
	if (alignment == nullptr) return;
	alignment->sortByTree(tree);
}


/*
 * Apply the filtering to this tree
 */
void PhylogenyAPI::applyFiltering(Filtering* filtering){
	if (PhylogenyAPI::THE_TREE == nullptr || filtering == nullptr) return;
	PhylogenyAPI::THE_TREE->applyFiltering(filtering);
}


/*
 * Height of current tree
 */
double PhylogenyAPI::getHeight(){
	return PhylogenyAPI::THE_TREE->getHeight();
}

bool PhylogenyAPI::isReady(){
	return PhylogenyAPI::THE_TREE != nullptr;
}




void PhylogenyAPI::cleanup(){
	if (PhylogenyAPI::THE_TREE != nullptr){
		PhylogenyAPI::THE_TREE->cleanup();
		delete PhylogenyAPI::THE_TREE;
		PhylogenyAPI::THE_TREE = nullptr;
	}

	AlignmentAPI::filtering = nullptr;
	AlignmentAPI::selectionIsDirty = false;
	AlignmentAPI::mostRecentlySelectedTaxon = -1;
}


/**
 * Prepare the labelling between tree and alignment if necessary
 */
void PhylogenyAPI::prepareLabelling(Alignment* alignment) {
	if (THE_TREE == nullptr) return;
	if (!orderingIsDirty) return;
	if (alignment == nullptr) {
		AlignmentAPI::makeMockAlignment(THE_TREE);
	}else{
		PhylogenyAPI::sortTaxaByTree(THE_TREE, alignment);
	}


	AlignmentAPI::setOrderingToDirty();
	orderingIsDirty = false;
}

/*
 * Generate graphics for the tree
 */
jsonObject PhylogenyAPI::getTreeGraphics(Scaling* scaling, double branchWidth, bool showTaxaOnTree, double nodeRadius,
									string branchColourBy, string nodeColourBy, double fontSize, int rounding,
									bool transmissionTree, Timeline* timeline, bool displayIncompatibleTransmissions,
									string branchCol, string nodeCol){

	return PhylogenyAPI::THE_TREE->getTreeGraphics(scaling, branchWidth, AlignmentAPI::getFiltering(), showTaxaOnTree,
				nodeRadius, branchColourBy, nodeColourBy, fontSize, rounding, transmissionTree, timeline, displayIncompatibleTransmissions,
				branchCol, nodeCol);


}


/*
 * Build a tree from the alignment using the specified method
 */
jsonObject PhylogenyAPI::buildTree(Alignment* alignment, LinkType method) {


	//String str = new String(contents);
	//cout << "Creating tree " << method << endl;

	PhylogenyAPI::cleanup();


	auto start = high_resolution_clock::now();


	// Build tree
	THE_TREE = new ClusterTree(alignment, method);


	// Sort taxa by tree
	sortTaxaByTree(THE_TREE, alignment);
	AlignmentAPI::setOrderingToDirty();
	EpiAPI::setEpiAnnotationsToDirty();
	EpiAPI::addAnnotationsToTree(THE_TREE);
	PhylogenyAPI::orderingIsDirty = false;


	// Prepare tree annotation options
	OptionsAPI::prepareTreeAnnotationOptions();
	OptionsAPI::resetScroll();

	auto finish = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(finish - start);



	jsonObject j;
	j["time"] = duration.count();
	j["newick"] = THE_TREE->toNewick();
	return j;



}



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
		WasmAPI::messageFromWasmToJS("");
	}


	void EMSCRIPTEN_KEEPALIVE uploadTree() {

		char* str = WasmAPI::getFromHeap();

		PhylogenyAPI::cleanup();

		cout << "uploading tree " << strlen(str) << endl;


		string contents(str);


		auto start = high_resolution_clock::now();



		PhylogenyAPI::THE_TREE = new Tree();
		PhylogenyAPI::THE_TREE->parseFromNexus(contents);
		PhylogenyAPI::orderingIsDirty = true;


		if (Error::wasError()) {
			PhylogenyAPI::cleanup();
			return;
		}

		// If alignment has been uploaded, check it matches the tree
		PhylogenyAPI::prepareLabelling(AlignmentAPI::getAlignment());

		// Prepare tree annotation options
		OptionsAPI::prepareTreeAnnotationOptions();
		OptionsAPI::resetScroll();

		// Epidemiological annotations
		EpiAPI::setEpiAnnotationsToDirty();
		EpiAPI::addAnnotationsToTree(PhylogenyAPI::THE_TREE);


		auto finish = high_resolution_clock::now();
		auto duration = duration_cast<seconds>(finish - start);




		cout << "Parsed tree successfully (" << duration.count() << "s)" << endl;


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
			PhylogenyAPI::orderingIsDirty = true;
		}
		WasmAPI::messageFromWasmToJS("{}");

	}








}

