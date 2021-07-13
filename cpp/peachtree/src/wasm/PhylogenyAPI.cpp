/*
 * PhylogenyAPI.cpp
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include "PhylogenyAPI.h"
#include "AlignmentAPI.h"
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


/**
 * Prepare the labelling between tree and alignment if necessary
 */
void PhylogenyAPI::prepareLabelling(Alignment* alignment) {
	if (alignment == nullptr) return;
	if (THE_TREE == nullptr) return;
	if (!orderingIsDirty) return;
	PhylogenyAPI::sortTaxaByTree(THE_TREE, alignment);
	AlignmentAPI::setOrderingToDirty();
	orderingIsDirty = false;
}


/*
 * Generate graphics for the tree
 */
jsonObject PhylogenyAPI::getTreeGraphics(Scaling* scaling, double branchWidth, bool showTaxaOnTree, double nodeRadius,
									string internalLabel, string leafLabel, double fontSize, int rounding,
									bool transmissionTree){

	return PhylogenyAPI::THE_TREE->getTreeGraphics(scaling, branchWidth, AlignmentAPI::getFiltering(), showTaxaOnTree, nodeRadius, internalLabel, leafLabel, fontSize, rounding, transmissionTree);


}


/*
 * Build a tree from the alignment using the specified method
 */
jsonObject PhylogenyAPI::buildTree(Alignment* alignment, LinkType method) {


	//String str = new String(contents);
	//cout << "Creating tree " << method << endl;


	auto start = high_resolution_clock::now();


	// Build tree
	THE_TREE = new ClusterTree(alignment, method);


	// Sort taxa by tree
	sortTaxaByTree(THE_TREE, alignment);
	AlignmentAPI::setOrderingToDirty();
	// TODO EpiAPI.setEpiAnnotationsToDirty();
	// TODO EpiAPI.addAnnotationsToTree(THE_TREE);
	PhylogenyAPI::orderingIsDirty = false;


	// Prepare tree annotation options
	OptionsAPI::prepareTreeAnnotationOptions();
	OptionsAPI::resetScroll();

	auto finish = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(finish - start);



	jsonObject j;
	j["time"] = duration.count();
	j["newick"] = THE_TREE->toNewick();
	return j;



}



// Interface between javascript and cpp for webassembly
extern "C" {

	void EMSCRIPTEN_KEEPALIVE uploadTree() {

		char* str = WasmAPI::getFromHeap();
		AlignmentAPI::filtering = nullptr;
		AlignmentAPI::selectionIsDirty = false;
		AlignmentAPI::mostRecentlySelectedTaxon = -1;

		cout << "uploading tree " << strlen(str) << endl;


		string contents(str);


		auto start = high_resolution_clock::now();

		PhylogenyAPI::THE_TREE = new Tree();
		PhylogenyAPI::THE_TREE->parseFromNexus(contents);
		PhylogenyAPI::orderingIsDirty = true;




		// If alignment has been uploaded, check it matches the tree
		PhylogenyAPI::prepareLabelling(AlignmentAPI::getAlignment());

		// Prepare tree annotation options
		OptionsAPI::prepareTreeAnnotationOptions();
		OptionsAPI::resetScroll();

		// Epidemiological annotations TODO
		// EpiAPI.setEpiAnnotationsToDirty();
		// EpiAPI.addAnnotationsToTree(THE_TREE);


		auto finish = high_resolution_clock::now();
		auto duration = duration_cast<seconds>(finish - start);


		if (Error::wasError()) {
			PhylogenyAPI::THE_TREE = nullptr;
			return;
		}

		cout << "Parsed tree successfully (" << duration.count() << "s)" << endl;


		//cout << "tree " << PhylogenyAPI::THE_TREE->toNewick() << endl;

		json j;
		j["time"] = duration.count();
		WasmAPI::messageFromWasmToJS(j.dump(0));



	}








}

