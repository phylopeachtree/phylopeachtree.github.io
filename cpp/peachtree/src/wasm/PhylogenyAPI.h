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
	static jsonObject getTreeGraphics(Scaling* scaling, double branchWidth, bool showTaxaOnTree, double nodeRadius, string internalLabel, string leafLabel, double fontSize, int rounding, bool transmissionTree);
	static jsonObject buildTree(Alignment* alignment, LinkType method);

	//Variables
	static Tree* THE_TREE;
	static bool orderingIsDirty;

};

#endif /* WASM_PHYLOGENYAPI_H_ */