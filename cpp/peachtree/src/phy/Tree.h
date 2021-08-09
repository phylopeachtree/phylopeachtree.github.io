/*
 * Tree.h
 *
 *  Created on: 6/07/2021
 *      Author: jdou557
 */


#ifndef PHY_TREE_H_
#define PHY_TREE_H_



#include <iostream>
#include "Node.h"
#include "../lib/json.hpp"
#include "../aln/Alignment.h"
#include "../aln/Taxon.h"
#include "../options/Scaling.h"


using namespace std;
using jsonObject = nlohmann::json;

class Filtering;
class Timeline;
class Tree {
public:

	Tree();
	Tree(Node* root, Alignment* alignment);

	bool getParsedFromFile();
	void parseFromNexus(string nexus);
	void setRoot(Node* root);
	Node* getRoot();
	Tree* copy();
	void initArray();
	vector<Node*> getNodesAsArray();
	vector<Node*> getLeavesAsArray();
	int getLeafNodeCount();
	string toNewick();
	string toString();
	vector<Node*> listNodes(Node* theRoot);
	static int getNodesPostOrder(Node* node, vector<Node*>* nodes, int pos);
	jsonObject getLadderGraphics(string label, double top, double left, double stepHeight, double stepWidth, int ladderSize, double min, double max, Node* subtree, string maxCol);
	jsonObject getTreeGraphics(Scaling* scaling, double branchWidth, Filtering* filtering, bool showTaxaOnTree,
								double nodeRadius, string branchColourBy, string nodeColourBy, double fontSize, int rounding, bool transmissionTree,
								Timeline* timeline, bool displayIncompatibleTransmissions, string branchCol, string nodeCol);
	double getHeight();
	void applyFiltering(Filtering* filtering);
	vector<Taxon*> getClade(vector<Taxon*> taxa);
	Node* getMRCA(vector<Taxon*> taxa);
	Node* getNode(Taxon* taxon);
	Node* getMRCA(Node* node1, Node* node2);
	vector<string> getAllAnnotations();
	void flipSubtree(int index);
	void cleanup();



protected:
	Node* root;
	vector<Node*> nodes;
	Alignment* alignment;
	bool parsedFromFile;

};

#endif /* PHY_TREE_H_ */



