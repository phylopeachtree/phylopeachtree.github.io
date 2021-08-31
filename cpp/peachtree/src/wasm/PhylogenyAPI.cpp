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
vector<Tree*> PhylogenyAPI::allTrees;
int PhylogenyAPI::treeNumber = -1;
bool PhylogenyAPI::orderingIsDirty;
bool PhylogenyAPI::infectionCountIsDirty;




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
	
	for (Tree* tree : PhylogenyAPI::allTrees){
		tree->cleanup();
		delete tree;
	}
	PhylogenyAPI::allTrees.clear();
	PhylogenyAPI::THE_TREE = nullptr;
	PhylogenyAPI::treeNumber = -1;


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
	}
	for (Tree* tree : PhylogenyAPI::allTrees){
		PhylogenyAPI::sortTaxaByTree(tree, alignment);
	}
		
	


	AlignmentAPI::setOrderingToDirty();
	orderingIsDirty = false;
}

/*
 * Generate graphics for the tree
 */
jsonObject PhylogenyAPI::getTreeGraphics(Scaling* scalingTree, Scaling* scalingLadder, double branchWidth, bool showTaxaOnTree, double nodeRadius,
									string branchColourBy, string nodeColourBy, double fontSize, int rounding,
									bool transmissionTree, Timeline* timeline, bool displayIncompatibleTransmissions,
									string branchCol, string nodeCol){

	return PhylogenyAPI::THE_TREE->getTreeGraphics(scalingTree, scalingLadder, branchWidth, AlignmentAPI::getFiltering(), showTaxaOnTree,
				nodeRadius, branchColourBy, nodeColourBy, fontSize, rounding, transmissionTree, timeline, displayIncompatibleTransmissions,
				branchCol, nodeCol);


}




/*
* Reorder nodes in the tree
*/
void PhylogenyAPI::reorderTree(Timeline* timeline, string symptomDateVar) {
	
	if (THE_TREE == nullptr) return;
	if (timeline == nullptr) return;
	if (!timeline->isReady()) return;
	//THE_TREE->reorderTree(timeline, symptomDateVar);
	
	// Count again
	for (Tree* tree : allTrees){
		tree->reorderTree(timeline, symptomDateVar);
	}
	
	
}


/*
 * Count the number of infections cased by each case
 */
void PhylogenyAPI::countInfections(){
	if (THE_TREE == nullptr) return;
	if (!PhylogenyAPI::infectionCountIsDirty) return;
	
	// Reset count to 0
	THE_TREE->resetInfections();
	
	// Count again
	for (Tree* tree : allTrees){
		tree->countInfections();
	}
	
	
	// Divide by num trees
	THE_TREE->normaliseInfections(allTrees.size());
	
	PhylogenyAPI::infectionCountIsDirty = false;
}


/*
 * Number of trees
 */
int PhylogenyAPI::getNumberOfTrees(){
	return PhylogenyAPI::allTrees.size();
}


/*
 * Build a tree from the alignment using the specified method
 */
jsonObject PhylogenyAPI::buildTree(Alignment* alignment, LinkType method) {

	auto start = high_resolution_clock::now();
	jsonObject j;
	if (!AlignmentAPI::isMock()){
		//String str = new String(contents);
		//cout << "Creating tree " << method << endl;

		PhylogenyAPI::cleanup();


		


		// Build tree
		THE_TREE = new ClusterTree(alignment, method);
		
		PhylogenyAPI::allTrees.push_back(THE_TREE);
		PhylogenyAPI::treeNumber = 0;
		

		// Sort taxa by tree
		sortTaxaByTree(THE_TREE, alignment);
		AlignmentAPI::setOrderingToDirty();
		EpiAPI::setEpiAnnotationsToDirty();
		EpiAPI::addAnnotationsToTree(THE_TREE);
		PhylogenyAPI::orderingIsDirty = false;
		PhylogenyAPI::infectionCountIsDirty = true;

		// Prepare tree annotation options
		OptionsAPI::prepareTreeAnnotationOptions();
		OptionsAPI::resetScroll();

	

		j["newick"] = THE_TREE->toNewick();

	}

	auto finish = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(finish - start);
	j["time"] = duration.count();
	
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


		// Epidemiological annotations
		EpiAPI::setEpiAnnotationsToDirty();
		for (Tree* tree : PhylogenyAPI::allTrees){
			EpiAPI::setEpiAnnotationsToDirty();
			EpiAPI::addAnnotationsToTree(tree);
			AlignmentAPI::annotateTaxa(tree);
		}
		
		
		
		
		
		
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
			PhylogenyAPI::orderingIsDirty = true;
			PhylogenyAPI::infectionCountIsDirty = true;
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

