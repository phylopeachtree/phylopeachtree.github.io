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
	static vector<Tree*> parseTrees(string contents);
	void parseFromNewick(string nexus, map<string, string> translateMap);
	void setRoot(Node* root);
	Node* getRoot();
	Tree* copy();
	void initArray();
	vector<Node*> getNodesAsArray();
	vector<Node*> getLeavesAsArray();
	int getLeafNodeCount();
	string toNewick();
	string toNewick(Filtering* filtering);
	string toString();
	static vector<Node*> listNodes(Node* theRoot);
	static int getNodesPostOrder(Node* node, vector<Node*>* nodes, int pos);
	
	jsonObject getTreeGraphics(Scaling* scalingTree, Scaling* scalingLadder, double branchWidth, Filtering* filtering, bool showTaxaOnTree,
								double nodeRadius, string branchColourBy, string nodeColourBy, double fontSize, int rounding, bool transmissionTree,
								Timeline* timeline, bool displayIncompatibleTransmissions, string branchCol, string nodeCol);
	double getHeight();
	void applyFiltering(Filtering* filtering);
	vector<Taxon*> getClade(vector<Taxon*> taxa);
	Node* getMRCA(vector<Taxon*> taxa);
	Node* getNode(Taxon* taxon);
	Node* getSubtree(vector<Taxon*> taxa);
	Node* getMRCA(Node* node1, Node* node2);
	vector<string> getAllAnnotations();
	void flipSubtree(int index);
	void cleanup();
	void reorderTree(Timeline* timeline, string symptomDateVar);
	void countInfections();
	void resetInfections();
	void normaliseInfections(int numTrees);



protected:
	Node* root;
	vector<Node*> nodes;
	Alignment* alignment;
	bool parsedFromFile;
	jsonObject getLadderGraphics(string label, double top, double left, double stepHeight, double stepWidth, int ladderSize, double min, double max, Node* subtree, string maxCol);
};

#endif /* PHY_TREE_H_ */



