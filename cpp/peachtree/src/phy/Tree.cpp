/*
 * Tree.cpp
 *
 *  Created on: 6/07/2021
 *      Author: jdou557
 */

#include "Tree.h"
#include "../Utils.h"
#include "../aln/Filtering.h"
#include "../error/Error.h"
#include "../epi/Timeline.h"


Tree::Tree(){
	this->alignment = nullptr;
	this->parsedFromFile = false;
	this->root = nullptr;
}

Tree::Tree(Node* root, Alignment* alignment){
	this->alignment = alignment;
	this->setRoot(root);
	this->parsedFromFile = false;
}



bool Tree::getParsedFromFile(){
	return this->parsedFromFile;
}


/*
 * Cleanup
 */
void Tree::cleanup(){

	for (Node* node : nodes){
		//node->cleanup();
		delete node;
	}

}


/*
 * Parse from nexus/newick
 */
void Tree::parseFromNexus(string nexus){



	this->parsedFromFile = true;


	// Parse fasta
	nexus.erase(std::remove(nexus.begin(), nexus.end(), '\r'), nexus.end());
	vector<string> lines = Utils::split(nexus, "\n");

	map<string, string> translateMap;

	// Get the translate tokens
	bool beganTrees = false;
	bool beganTranslate = false;
	vector<string> splt;
	for (int lineNum = 0; lineNum < lines.size(); lineNum++) {
		string line = lines.at(lineNum);
		Utils::trim(line);


		// Commenting
		if (line.substr(0, 1) == "#") continue;



		if (Utils::toLower(line).find("begin trees;") != string::npos) {
			beganTrees = true;
		}
		else if (beganTrees && line == ";") {
			break;
		}
		else if (beganTrees && !beganTranslate && Utils::toLower(line).find("translate") != string::npos) {
			beganTranslate = true;
		}
		else if (beganTranslate) {

			// Translate token
			splt =  Utils::split(line, " ");
			if (splt.size() < 2) {
				Error::throwError("Cannot parse translate line " + line + " because there are not 2 tokens split by a single space");
				return;
			}

			string id = splt.at(0);
			string label = "";
			for (int i = 1; i < splt.size(); i ++) {
				label += splt.at(i);
				if (i < splt.size()-1) label += " ";
			}

			// Remove trailing comma
			if (label.substr(label.size()-1) == ",") label = label.substr(0, label.size()-1);


			// Remove " from start and end if there are spaces in the label
			if (splt.size() > 2) {
				if (label.substr(0, 1) == "\"" && label.substr(label.length()-1, label.length()) == "\"") {
					label = label.substr(1, label.length()-1);
				}
			}



			if (translateMap.count(id) > 0) {
				Error::throwError("Duplicate translate id detected " + id);
				return;
			}
			translateMap[id] = label;

		}

	}


	// Get the last tree's newick
	string newick = "";
	for (int lineNum = lines.size()-1; lineNum >= 0; lineNum --) {
		//String[] lineSplit = lines[lineNum].split("[=]", 2);


		vector<string> lineSplit = Utils::split(lines.at(lineNum), "=", 2);
		if (lineNum == lines.size()-2) {
			//if (lineSplit[0].length() >= 4) System.out.println(lineSplit[0].toLowerCase().substring(0, 4));
		}
		if (lineSplit.size() == 2 && lineSplit.at(0).length() >= 4 && Utils::toLower(lineSplit.at(0)).substr(0, 4) == "tree") {
			newick = lineSplit.at(1);
			Utils::trim(newick);
			break;
		}
	}




	// Maybe it's just newick not nexus. Take first non empty line
	if (newick.empty()) {

		for (int lineNum = 0; lineNum < lines.size(); lineNum ++) {
			if (!lines.at(lineNum).empty()) {
				newick = lines.at(lineNum);
				break;
			}
		}

		if (newick.empty()) {
			Error::throwError("Cannot locate any newick trees in file");
			return;
		}
	}




	// Remove trailing ; and all spaces
	newick.erase(remove(newick.begin(), newick.end(), ';'), newick.end());
	newick.erase(remove(newick.begin(), newick.end(), ' '), newick.end());



	this->root = new Node();
	this->root->setHeight(0);
	this->root->parseFromNewick(newick);
	this->initArray();


	cout << "333" << endl;

	// Normalise so that the smallest leaf is at height 0
	double minimalHeight = Utils::INFTY;
	for (Node* node : this->getNodesAsArray()) minimalHeight = std::min(minimalHeight, node->getHeight());
	for (Node* node : this->getNodesAsArray()) node->setHeight(node->getHeight() - minimalHeight);


	// Apply label translation
	if (!translateMap.empty()) {
		for (Node* node : this->getNodesAsArray()) {
			string key = node->getAcc();
			if (translateMap.count(key) > 0) {
				//cout << key << " has accession " << translateMap[key] << " and height " << node->getHeight() << endl;
				node->setAcc(translateMap[key]);
			}
		}
	}


}



void Tree::setRoot(Node* root){
	this->root = root;
	this->initArray();
}

Node* Tree::getRoot(){
	return this->root;
}

Tree* Tree::copy(){
	return new Tree(this->root->copy(), this->alignment);
}


/*
 * Post-order traversal
 */
void Tree::initArray(){
	this->nodes = this->listNodes(this->root);
	for (int i = 0; i < this->nodes.size(); i ++) {
		this->nodes.at(i)->setNr(i);
	}
}


/*
 * List of nodes, where first indices are leaves, then internal nodes, then root
 */
vector<Node*> Tree::getNodesAsArray(){
	return this->nodes;
}

/*
 * List of leaves i.e. the first N nodes
 */
vector<Node*> Tree::getLeavesAsArray(){
	vector<Node*> leaves(this->getLeafNodeCount());
	for (int i = 0; i < leaves.size(); i ++) {
		leaves.at(i) = this->nodes.at(i);
	}
	return leaves;
}


/*
 * Number of leaf nodes
 */
int Tree::getLeafNodeCount(){
	return this->root->getLeafNodeCount();
}

/*
 * To newick string
 */
string Tree::toNewick(){
	int dummy = 0;
	return this->root->toSortedNewick(dummy, true) + ")";
}

string Tree::toString(){
	return this->toNewick();
}

/*
 * Reset the node ordering
 */
vector<Node*> Tree::listNodes(Node* theRoot){


	vector<Node*> toReturn(theRoot->getNodeCount());

	// Post order traversal
	vector<Node*>* nodeArr = new vector<Node*>(theRoot->getNodeCount());
	Tree::getNodesPostOrder(theRoot, nodeArr, 0);

	// Leaves first, then internal, then root
	int index = 0;
	for (int i = 0; i < nodeArr->size(); i ++){
		Node* node = nodeArr->at(i);
		if (node->isLeaf()) {
			toReturn.at(index) = node;
			index++;
		}
	}
	for (int i = 0; i < nodeArr->size(); i ++){
		Node* node = nodeArr->at(i);
		if (!node->isLeaf() && !node->isRoot()) {
			toReturn.at(index) = node;
			index++;
		}
	}
	for (int i = 0; i < nodeArr->size(); i ++){
		Node* node = nodeArr->at(i);
		if (!node->isLeaf() && node->isRoot()) {
			toReturn.at(index) = node;
			index++;
		}
	}

	return toReturn;


}

int Tree::getNodesPostOrder(Node* node, vector<Node*>* nodes, int pos){

	 if (!node->isLeaf()) {
		 for (Node* child : node->getChildren()) {
			 pos = getNodesPostOrder(child, nodes, pos);
		 }
	 }
	 nodes->at(pos) = node;
	 return pos + 1;
}


/*
 * Get a json array of graphics
 */
jsonObject Tree::getTreeGraphics(Scaling* scaling, double branchWidth, Filtering* filtering, bool showTaxaOnTree,
						double nodeRadius, string internalLabel, string leafLabel, double fontSize, int rounding, bool transmissionTree, Timeline* timeline){


	jsonObject objs = json::array();
	if (!scaling->inView()) return objs;

	// Calculate yshift to avoid clipping top margin
	// First, the top most leaf in the view must be found
	double yshift = 0;
	for (int leafNr = 0; leafNr < this->getLeafNodeCount(); leafNr ++) {
		int filteredNr = this->nodes.at(leafNr)->getFilteredNr();
		if (filteredNr >= 0 && scaling->inRangeY(filteredNr)){
			yshift = scaling->getCanvasMinY() - scaling->scaleY(filteredNr);
			break;
		}
	}

	Node* subtree = filtering->getSubtreeRoot() != nullptr ? filtering->getSubtreeRoot() : this->root;
	subtree->getGraphics(true, objs, filtering, scaling, branchWidth, showTaxaOnTree, yshift, nodeRadius,
				internalLabel, leafLabel, fontSize, rounding, transmissionTree, timeline);
	return objs;


}

double Tree::getHeight(){
	return this->root->getHeight();
}


/*
 * Apply the taxon filtering
 */
void Tree::applyFiltering(Filtering* filtering){

	if (filtering == nullptr) return;

	int nr = 0;
	for (Node* leaf : this->getLeavesAsArray()) {

		// Maintain a separate numbering system where filtered-out leaves are omitted
		if (filtering->getFocusing()) {
			if (leaf->getTaxon()->getIsSelected()) {
				leaf->setFilteredNr(nr);
				nr ++;
			}else {
				leaf->setFilteredNr(-1);
			}
		}else {
			leaf->setFilteredNr(nr);
			nr ++;
		}



	}

}


/*
 * Get all taxa which form a clade
 */
vector<Taxon*> Tree::getClade(vector<Taxon*> taxa){

	// Find the mrca
	Node* mrca = getMRCA(taxa);


	// Get all leaves in this subtree
	vector<Node*> leaves;
	mrca->getLeafSet(leaves);


	// Return their taxa
	vector<Taxon*> clade;
	for (Node* leaf : leaves) clade.push_back(leaf->getTaxon());
	return clade;

}


/*
 * Get the MRCA of a list of taxa
 */
Node* Tree::getMRCA(vector<Taxon*> taxa){

	Node* mrca = nullptr;
	for (int i = 0; i < taxa.size(); i ++) {
		Node* node1 = this->getNode(taxa.at(i));
		for (int j = i+1; j < taxa.size(); j ++) {
			Node* node2 = this->getNode(taxa.at(j));
			Node* ancestor = Tree::getMRCA(node1, node2);

			// The mrca of the set of taxa is the pairwise mrca with the greatest height
			if (mrca == nullptr || ancestor->getHeight() > mrca->getHeight()){
				mrca = ancestor;
			}

		}
	}

	return mrca;
}


/*
 * Returns the leaf node associated with this taxon
 */
Node* Tree::getNode(Taxon* taxon){
	for (Node* leaf : this->getLeavesAsArray()) {
		if (leaf->getTaxon() == taxon) return leaf;
	}
	return nullptr;
}


/*
 * Gets thr MRCA of these two nodes
 */
Node* Tree::getMRCA(Node* node1, Node* node2){

	// Trace from node1 to the root
	vector<Node*> trace1;
	Node* node = node1;
	while (node != nullptr) {
		trace1.push_back(node);
		node = node->getParent();
	}

	// Find the earliest node on this path from node2 to the root
	node = node2;
	while(node != nullptr) {
		if (std::count(trace1.begin(), trace1.end(), node) > 0) return node;
		//if (trace1.contains(node))
		node = node->getParent();
	}

	return nullptr;
}


/*
 * Find all annotation variables and sort them alphabetically
 */
vector<string> Tree::getAllAnnotations(){
	vector<string> annotations;
	root->getAllAnnotations(annotations);
	std::sort(annotations.begin(), annotations.end());
	return annotations;
}


/*
 * Rotate the subtree at the specified internal node
 */
void Tree::flipSubtree(int index){

	// Find the node
	if (index < 0 || index >= this->nodes.size()) return;
	Node* node = this->nodes.at(index);
	if (node->isLeaf()) return;
	node->rotateChildren();

	this->initArray();


}


