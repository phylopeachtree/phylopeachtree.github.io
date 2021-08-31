/*
 * PhylogenyAPI.h
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */


#include <iostream>
#include "WasmAPI.h"
#include "../phy/Tree.h"
#include "../phy/ClusterTree.h"
#include "../epi/Timeline.h"

using namespace std;

#ifndef WASM_PHYLOGENYAPI_H_
#define WASM_PHYLOGENYAPI_H_

class PhylogenyAPI {
public:

	// Methods
	static Tree* getTree();
	static vector<string> getAllAnnotations();
	static void prepareLabelling(Alignment* alignment);
	static void sortTaxaByTree(Tree* tree, Alignment* alignment);
	static bool isReady();
	static void applyFiltering(Filtering* filtering);
	static double getHeight();
	static jsonObject getTreeGraphics(Scaling* scalingTree, Scaling* scalingLadder, double branchWidth, bool showTaxaOnTree, double nodeRadius,
			string branchColourBy, string nodeColourBy, double fontSize, int rounding, bool transmissionTree, Timeline* timeline, bool displayIncompatibleTransmissions,
			string branchCol, string nodeCol);
	static jsonObject buildTree(Alignment* alignment, LinkType method);
	static void cleanup();
	static void setOrderingToDirty();
	static void reorderTree(Timeline* timeline, string symptomDateVar);
	static void countInfections();
	static int getNumberOfTrees();

	//Variables
	static Tree* THE_TREE;
	static vector<Tree*> allTrees;
	static int treeNumber;
	static bool orderingIsDirty;
	static bool infectionCountIsDirty;

};

#endif /* WASM_PHYLOGENYAPI_H_ */
