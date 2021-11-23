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
#include <chrono>


using namespace std;
using namespace std::chrono;
using json = nlohmann::json;


Tree* PhylogenyAPI::THE_TREE;
vector<Tree*> PhylogenyAPI::allTrees;
int PhylogenyAPI::treeNumber = -1;
bool PhylogenyAPI::orderingIsDirty;
bool PhylogenyAPI::infectionCountIsDirty;


void PhylogenyAPI::setTree(Tree* tree){
	allTrees.clear();
	THE_TREE = tree;
	if (tree != nullptr){
		allTrees.push_back(tree);
		treeNumber = 0;
	}else{
		treeNumber = -1;
	}
	orderingIsDirty = true;
	infectionCountIsDirty = true;
}

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
									ColourOption* branchCol, ColourOption* nodeCol){

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


