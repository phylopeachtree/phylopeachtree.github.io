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
 * Parse a list of trees
 */
vector<Tree*> Tree::parseTrees(string nexus){
	
	

	const bool justonetree = true;
	
	vector<Tree*> trees;
	
	// Parse nexus
	nexus.erase(std::remove(nexus.begin(), nexus.end(), '\r'), nexus.end());
	vector<string> lines = Utils::split(nexus, "\n");

	map<string, string> translateMap;

	// Get the translate tokens
	bool beganTrees = false;
	bool beganTranslate = false;
	vector<string> splt;
	int lineNum = 0;
	for (lineNum = 0; lineNum < lines.size(); lineNum++) {
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
				return trees;
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
					label = label.substr(1, label.length()-2);
				}
			}



			if (translateMap.count(id) > 0) {
				Error::throwError("Duplicate translate id detected " + id);
				return trees;
			}
			translateMap[id] = label;
			
			//cout << id << "=" << label << endl;

		}

	}


	

	// Get newicks
	if (!justonetree) {
		for (lineNum = lineNum; lineNum < lines.size(); lineNum ++) {

			vector<string> lineSplit = Utils::split(lines.at(lineNum), "=", 2);
			if (lineSplit.size() == 2 && lineSplit.at(0).length() >= 4 && Utils::toLower(lineSplit.at(0)).substr(0, 4) == "tree") {
				string newick = lineSplit.at(1);
				Utils::trim(newick);
				
				Tree* tree = new Tree();
				tree->parseFromNewick(newick, translateMap);
				trees.push_back(tree);
				
			}
		}
	}else{
		for (lineNum = lines.size()-1; lineNum >= 0; lineNum --) {

			vector<string> lineSplit = Utils::split(lines.at(lineNum), "=", 2);
			if (lineSplit.size() == 2 && lineSplit.at(0).length() >= 4 && Utils::toLower(lineSplit.at(0)).substr(0, 4) == "tree") {
				string newick = lineSplit.at(1);
				Utils::trim(newick);
				
				Tree* tree = new Tree();
				tree->parseFromNewick(newick, translateMap);
				trees.push_back(tree);
				
				break;
				
			}
		}
		
	}
	
	


	// Maybe it's just newick not nexus. Take first non empty line
	if (trees.empty()) {

		for (lineNum = 0; lineNum < lines.size(); lineNum ++) {
			if (!lines.at(lineNum).empty()) {
				string newick = lines.at(lineNum);
				Tree* tree = new Tree();
				tree->parseFromNewick(newick, translateMap);
				trees.push_back(tree);
			}
		}
	}


	if (trees.empty()) {
		Error::throwError("Cannot locate any newick trees in file");
		return trees;
	}

	
	
	return trees;

	
}


/*
 * Parse from nexus/newick
 */
void Tree::parseFromNewick(string newick, map<string, string> translateMap){


	//cout << "parsing newick" << newick << endl;

	this->parsedFromFile = true;



	// Remove trailing ; and all spaces
	newick.erase(remove(newick.begin(), newick.end(), ';'), newick.end());
	newick.erase(remove(newick.begin(), newick.end(), ' '), newick.end());




	this->root = new Node();
	this->root->setHeight(0);
	this->root->parseFromNewick(newick);
	
	this->initArray();
	


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
	this->nodes = Tree::listNodes(this->root);
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
	int dummy = -1;
	vector<string>* labels = new vector<string>();
	string str = this->root->toSortedNewick(dummy, true, labels);
	delete labels;
	return str;
}


/*
 * To newick string
 */
string Tree::toNewick(Filtering* filtering){
	int dummy = -1;
	Node* subtree = this->root;
	if (filtering->getSubtreeRoot() != nullptr) subtree = filtering->getSubtreeRoot();
	vector<string>* labels = new vector<string>();
	string str = subtree->toSortedNewick(dummy, true, labels);
	delete labels;
	return str;
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
 * Get graphics for a colour ladder
 */
jsonObject Tree::getLadderGraphics(string label, double top, double left, double stepHeight, double stepWidth, int ladderSize, double min, double max, Node* subtree, vector<string> palette){

	json arr = json::array();

	const double textSize = stepHeight;
	const double textMargin = 2;


	// Plot a legend for node colouring
	for (int step = 0; step < ladderSize; step ++){

		double val = (1.0*ladderSize-step-1) / (ladderSize-1) * (max - min) + min;
		string col = subtree->getAnnotationColour(val, min, max, palette);


		// Background colour
		json step_json;
		step_json["ele"] = "rect";
		step_json["x"] = left + textSize;
		step_json["y"] = top + stepHeight*step;
		step_json["width"] = stepWidth;
		step_json["height"] = stepHeight;
		step_json["fill"] = col;
		step_json["stroke"] = "black";
		arr.push_back(step_json);


	}


	// Label
	json label_json;
	label_json["ele"] = "text";
	//label_json["x"] = left;
	//label_json["y"] = top - stepHeight/2;
	label_json["value"] = label;
	label_json["text_anchor"] = "start";
	label_json["font_size"] = textSize;
	label_json["transform"] = "translate(" + to_string(left) + "," + to_string(top) + ") rotate(90)";
	arr.push_back(label_json);


	// Max text
	json max_json;
	max_json["ele"] = "text";
	max_json["x"] = left + stepWidth + textMargin + textSize;
	max_json["y"] = top + stepHeight/2;
	max_json["value"] = Utils::roundToSF(max, 3);
	max_json["text_anchor"] = "start";
	max_json["font_size"] = textSize;
	//max_json["transform"] = "translate(" + to_string(left + stepWidth + textMargin + textSize) + "," + to_string(top + stepHeight/2) + ") rotate(90)";
	arr.push_back(max_json);


	// Min text
	json min_json;
	min_json["ele"] = "text";
	min_json["x"] = left + stepWidth + textMargin + textSize;
	min_json["y"] = top + stepHeight*(ladderSize-0.5);
	min_json["value"] =  Utils::roundToSF(min, 3);
	min_json["text_anchor"] = "start";
	min_json["font_size"] = textSize;
	//max_json["transform"] = "translate(" + to_string(left + stepWidth + textMargin + textSize) + "," + to_string(top + stepHeight*(ladderSize-0.5)) + ") rotate(90)";
	arr.push_back(min_json);


	return arr;

}



/*
 * Get a json array of graphics
 */
jsonObject Tree::getTreeGraphics(Scaling* scalingTree, Scaling* scalingLadder, double branchWidth, Filtering* filtering, bool showTaxaOnTree,
						double nodeRadius, string branchColourBy, string nodeColourBy, double fontSize, int rounding, bool transmissionTree,
						Timeline* timeline, bool displayIncompatibleTransmissions, ColourOption* branchCol, ColourOption* nodeCol){


	jsonObject objs = json::array();
	if (!scalingTree->inView()) return objs;

	// Calculate yshift to avoid clipping top margin
	// First, the top most leaf in the view must be found
	double yshift = 0;
	for (int leafNr = 0; leafNr < this->getLeafNodeCount(); leafNr ++) {
		int filteredNr = this->nodes.at(leafNr)->getFilteredNr();
		if (filteredNr >= 0 && scalingTree->inRangeY(filteredNr)){
			yshift = scalingTree->getCanvasMinY() - scalingTree->scaleY(filteredNr);
			break;
		}
	}
	Node* subtree = filtering->getSubtreeRoot() != nullptr ? filtering->getSubtreeRoot() : this->root;


	// Colourings? Get min and max and draw legend
	vector<double> minMaxNode({Utils::INFTY, -Utils::INFTY});
	vector<double> minMaxBranch({Utils::INFTY, -Utils::INFTY});


	// Draw colour legends?
	if (nodeColourBy != ""){
		subtree->getMinMax(nodeColourBy, minMaxNode);

		// Set 0 to origin if the min is above 0
		if (minMaxNode.at(0) > 0) minMaxNode.at(0) = 0;
	}
	if (branchColourBy != ""){
		subtree->getMinMax(branchColourBy, minMaxBranch);
		
		// Set 0 to origin if the min is above 0
		if (minMaxNode.at(0) > 0) minMaxNode.at(0) = 0;
		
	}
	
	
	
	


	subtree->getGraphics(true, objs, filtering, scalingTree, branchWidth, showTaxaOnTree, yshift, nodeRadius,
			branchColourBy, nodeColourBy, fontSize, rounding, transmissionTree, timeline, displayIncompatibleTransmissions,
			branchCol, nodeCol, minMaxNode, minMaxBranch);
			
			
			
	if (scalingLadder != nullptr){
		
		// White bg
		jsonObject rect;
		rect["ele"] = "rect";
		rect["x"] = scalingLadder->getCanvasMinX();;
		rect["y"] = scalingLadder->getCanvasMinY();
		rect["width"] = scalingLadder->getCanvasMaxX() - scalingLadder->getCanvasMinX();
		rect["height"] = scalingLadder->getCanvasMaxY() - scalingLadder->getCanvasMinY();
		rect["fill"] = "white";
		objs.push_back(rect);
		
		
		// Draw colour legends
		int ladderSize = 0;
		const double ladderMargin = 4;
		const double ladderHeight = 11;
		const double ladderWidth = scalingLadder->getCanvasMaxX() - scalingLadder->getCanvasMinX() - 2*ladderMargin - 2*ladderHeight; 
		
		if (nodeColourBy != ""){
			
			vector<string> palette = nodeCol->getPalette();
			ladderSize = palette.size();
			

			double top = scalingLadder->getCanvasMinY() + ladderMargin;
			double left = scalingLadder->getCanvasMinX() + ladderMargin;
			jsonObject ladder = getLadderGraphics("Node " + nodeColourBy, top, left, ladderHeight, ladderWidth, ladderSize, minMaxNode.at(0), minMaxNode.at(1),  subtree, palette);
			objs.insert(objs.end(), ladder.begin(), ladder.end());
		}
		if (branchColourBy != ""){


			vector<string> palette = branchCol->getPalette();

			double top = scalingLadder->getCanvasMinY() + ladderMargin;
			if (nodeColourBy != "") top = scalingLadder->getCanvasMaxY() - ladderMargin - ladderHeight*(ladderSize+2);
			ladderSize = palette.size();
			
			double left = scalingLadder->getCanvasMinX() + ladderMargin;
			jsonObject ladder = getLadderGraphics("Branch " + branchColourBy, top, left, ladderHeight, ladderWidth, ladderSize, minMaxBranch.at(0), minMaxBranch.at(1), subtree, palette);
			objs.insert(objs.end(), ladder.begin(), ladder.end());
		}

		
	}
		

		

			

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
			if (filtering->includeTaxon(leaf->getTaxon())) {
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

	vector<Taxon*> clade;

	if (taxa.empty()) {
		clade.push_back(this->root->getTaxon());
		return clade;
	}

	if (taxa.size() == 1){
		return taxa;
	}

	// Find the mrca
	Node* mrca = getMRCA(taxa);
	

	// Get all leaves in this subtree
	vector<Node*> leaves;
	mrca->getLeafSet(leaves);
	

	// Return their taxa
	
	for (Node* leaf : leaves) clade.push_back(leaf->getTaxon());
	return clade;

}



/*
 * Get the subtree of a list of taxa
 */
Node* Tree::getSubtree(vector<Taxon*> taxa){
	
	// Find the mrca
	Node* root = getMRCA(taxa)->copy();
	
	root->getSubtree(taxa);

	
	return root;
	
}


/*
 * Get the MRCA of a list of taxa
 */
Node* Tree::getMRCA(vector<Taxon*> taxa){
	
	if (taxa.size() == 0) return this->root;
	if (taxa.size() == 1) return getNode(taxa.at(0));

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


/*
 * Reorder nodes in the transmission tree to maximise compatibility with infectious periods
 */
void Tree::reorderTree(Timeline* timeline, string symptomDateVar){
	if (timeline == nullptr || symptomDateVar == "") return;
	cout << "reordering tree" << endl;
	this->root->reorderTransmissions(timeline, symptomDateVar);
}

/*
 * Count the number of infections cased by each case
 */
void Tree::countInfections(){
	this->root->countInfections();
}

/*
 * Set num infections of each case to 0
 */
void Tree::resetInfections(){
	this->root->resetInfections();
}
/*
 * Divide num infections of each case by 'numTrees'
 */
void Tree::normaliseInfections(int numTrees){
	this->root->normaliseInfections(numTrees);
}



