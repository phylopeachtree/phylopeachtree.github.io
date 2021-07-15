/*
 * Node.cpp
 *
 *  Created on: 6/07/2021
 *      Author: jdou557
 */


#include <regex>

#include "Node.h"
#include "../Utils.h"
#include "../error/Error.h"


Node::Node(){
	this->acc = "";
	this->nodeNr = 0;
	this->filteredNodeNr = 0;
	this->height = 0;
	this->parent = nullptr;
	this->taxon = nullptr;
}


Node::Node(int index){
	this->acc = "";
	this->nodeNr = index;
	this->filteredNodeNr = 0;
	this->height = 0;
	this->parent = nullptr;
	this->taxon = nullptr;
}


/*
 * Get the accession of this taxon or node number if not applicable
 */
string Node::getAcc(){
	if (this->acc.empty()) return to_string(this->nodeNr+1);
	return this->acc;
}

void Node::setAcc(string acc){
	this->acc = acc;
}

void Node::setTaxon(Taxon* taxon){
	this->taxon = taxon;
}

Taxon* Node::getTaxon(){
	return this->taxon;
}

int Node::getNr(){
	return this->nodeNr;
}

void Node::setNr(int nr){
	this->nodeNr = nr;
}

void Node::Node::setFilteredNr(int nr){
	this->filteredNodeNr = nr;
}

int Node::getFilteredNr(){
	return this->filteredNodeNr;
}

void Node::setParent(Node* parent){
	this->parent = parent;
}

void Node::removeParent(){
	this->parent = nullptr;
}

double Node::getHeight(){
	return this->height;
}

void Node::setHeight(double h){
	this->height = h;
}

/*
 * Set the parent of this node
 */
Node* Node::getParent(){
	return this->parent;
}

void Node::addChild(Node* child){
	this->children.push_back(child);
	child->setParent(this);
}

bool Node::isLeaf(){
	return this->getNumChildren() == 0;
}

bool Node::isRoot(){
	return this->parent == nullptr;
}

Node* Node::getChild(int i){
	return this->children.at(i);
}

int Node::getNumChildren(){
	return this->children.size();
}

vector<Node*> Node::getChildren(){
	return children;
}


/*
 * Copy the node
 */
Node* Node::copy(){
	Node* clone = new Node(this->getNr());
	clone->acc = this->acc;
	clone->height = this->height;
	for (const auto &pair : this->annotations) {
		string key = pair.first;
		clone->annotations[key] = this->annotations[key];
	}
	for (Node* child : children) {
		clone->addChild(child->copy());
	}
	return clone;
}

/*
 * Size of subtree
 */
int Node::getNodeCount(){
	int size = 1;
	if (!this->isLeaf()) {
		for (Node* child : this->children) {
			size += child->getNodeCount();
		}
	}
	return size;
}


/*
 * Number of leaves in subtree
 */
int Node::getLeafNodeCount(){
	if (this->isLeaf()) return 1;
	int size = 0;
	for (Node* child : this->children) {
		size += child->getLeafNodeCount();
	}
	return size;
}

/*
 * Gets all leaves in this subtree and adds them to the list
 */
void Node::getLeafSet(vector<Node*>& leaves){
	if (this->isLeaf()) leaves.push_back(this);
	for (Node* child : this->children) {
		child->getLeafSet(leaves);
	}
}


/*
 * Cleanup
 */
void Node::cleanup(){
	// Nothing to do
}

/*
 * Number of immediate children
 */
int Node::getChildCount(){
	return this->children.size();
}

/*
 * Length of this branch
 */
double Node::getLength(){
	if (this->isRoot()) return 0;
	return this->getParent()->getHeight() - this->getHeight();
}

string Node::tostring(){
	int dummy = 0;
	return this->toSortedNewick(dummy, false);
}

/*
 * Print node as a newick
 * Adapted from beast2.evolution.tree.Node
 */
string Node::toSortedNewick(int& maxNodeInClade, bool printMetaData){
	string buf;


    if (!isLeaf()) {

        if (getChildCount() <= 2) {
            // Computationally cheap method for special case of <=2 children

            buf.append("(");
            string child1 = getChild(0)->toSortedNewick(maxNodeInClade, printMetaData);
            int child1Index = maxNodeInClade;
            if (getChildCount() > 1) {
                string child2 = getChild(1)->toSortedNewick(maxNodeInClade, printMetaData);
                int child2Index = maxNodeInClade;
                if (child1Index > child2Index) {
                    buf.append(child2);
                    buf.append(",");
                    buf.append(child1);
                } else {
                    buf.append(child1);
                    buf.append(",");
                    buf.append(child2);
                    maxNodeInClade = child1Index;
                }
            } else {
                buf.append(child1);
            }
            buf.append(")");
            //buf.append(this.getAcc());

        } else {
            // General method for >2 children

            vector<string> childStrings(getChildCount()); // = new String[getChildCount()];
            vector<int> maxNodeNrs(getChildCount());
            vector<int> indices(getChildCount());
            for (int i = 0; i < getChildCount(); i++) {
                childStrings.at(i) = getChild(i)->toSortedNewick(maxNodeInClade, printMetaData);
                maxNodeNrs.at(i) = maxNodeInClade;
                indices.at(i) = i;
            }


            std::sort(indices.begin(), indices.end());


            maxNodeInClade = maxNodeNrs.at(maxNodeNrs.size() - 1);

            buf.append("(");
            for (int i = 0; i < indices.size(); i++) {
                if (i > 0)
                    buf.append(",");

                buf.append(childStrings.at(indices.at(i)));
            }

            buf.append(")");
            //buf.append(this.getAcc());
        }

    } else {
        maxNodeInClade = nodeNr;
        //buf.append(nodeNr + 1);
    }

    buf.append(this->getAcc());

    if (printMetaData) {
        buf.append(getNewickMetaData());
    }
    buf.append(":");

    //if (printMetaData) buf.append(getNewickLengthMetaData());
    buf.append(to_string(getLength()));

    return buf;


}

/*
 * Meta data annotations for newick string
 */
string Node::getNewickMetaData(){

	if (this->annotations.empty()) return "";
	string str = "[&";
	int i = 0;
	for (const auto & pair : this->annotations) {
		string key = pair.first;
		string val = pair.second;
		str.append(key);
		str.append("=");
		str.append(val);
		if (i < this->annotations.size()-1) str.append(",");
		i++;
	}
	str.append("]");
	return str;

}

/*
 *  Meta data annotations for displaying string
 */
string Node::getTidyMetaData(){


	string str;
	if (this->isLeaf()) {
		str.append(this->getAcc());
	}
	else {
		str.append("Click node to switch child ordering.");
	}
	str.append("\nheight=");
	str.append(to_string(this->getHeight())); //OptionsAPI.sf(this->getHeight()));
	if (this->annotations.empty()) return str;
	str.append("\n");
	int i = 0;
	for (const auto & pair: this->annotations) {
		string key = pair.first;
		string value = pair.second;

		/* Rounding
		try {
    		vector<string> bits = Utils::split(value, ",");
    		string roundedValue = "";
    		for (int j = 0; j < bits.size(); j ++) {
    			double val = Double.parseDouble(bits[j]);
    			val = OptionsAPI.sf(val);
    			roundedValue += Double.toString(val);
    			if (j < bits.length-1) roundedValue += ", ";
    		}
    		value = roundedValue;

		}catch (Exception e) {

		}
		*/
		str.append(key).append("=").append(value);
		if (i < this->annotations.size()-1) str.append("\n");
		i++;
	}

	return str;

}


/*
 * Get the svg graphics of this subtree
 */
double Node::getGraphics(bool isRoot, jsonObject& objs, Filtering* filtering, Scaling* scaling, double branchWidth,
		bool showTaxaOnTree, double yshift, double nodeRadius, string internalLabel, string leafLabel, double fontSize, int rounding,
		bool transmissionTree){


	double x2 = this->getHeight();
	double x2Scaled = scaling->scaleX(x2);
	double maxY = -Utils::INFTY;
	double minY =  Utils::INFTY;

	// If leaf, go with node ordering
	double y = Utils::INFTY;
	int nValidChildren = 0;
	if (this->isLeaf()) {
		if (!filtering->includeTaxon(this->getTaxon())) return Utils::INFTY;
		y = this->getFilteredNr() + 0.5;
	}


	// If parent node, y is midpoint between children
	else {

		for (Node* child : this->getChildren()) {
			double ychild = child->getGraphics(false, objs, filtering, scaling, branchWidth, showTaxaOnTree, yshift, nodeRadius, internalLabel, leafLabel, fontSize, rounding, transmissionTree);
			if (ychild != Utils::INFTY) {
				if (y == Utils::INFTY) y = 0.0;
				y += ychild;
				if (ychild > maxY) maxY = ychild;
				if (ychild < minY) minY = ychild;
				nValidChildren ++;
			}
		}
		if (y != Utils::INFTY) {
			if (transmissionTree) y = minY;
			else y = y / nValidChildren;
		}

	}


	// If this has y=infty, then do not plot the subtree
	if (y == Utils::INFTY) return y;

	double yscaled = scaling->scaleY(y);
	yscaled += yshift;
	bool inrangeY = scaling->inRangeY(y);

	// Dashed line to taxa
	if (this->isLeaf() && showTaxaOnTree && this->getHeight() > 0) {

		// Only draw if this node is in y-range
		if (inrangeY) {

			jsonObject dashed_json;
			dashed_json["ele"] = "line";
			dashed_json["x1"] = x2Scaled;
			dashed_json["x2"] = scaling->getCanvasMaxX();
			dashed_json["y1"] = yscaled;
			dashed_json["y2"] = yscaled;
			dashed_json["stroke_width"] = branchWidth/2;
			dashed_json["stroke"] = "black";
			dashed_json["stroke_linecap"] = "round";
			dashed_json["stroke_dasharray"] = "4,7";
			objs.push_back(dashed_json);

		}

	}


	// Shoulder
	if (!this->isLeaf() && nValidChildren > 1) {


		// Only draw if this node is in y-range
		if (scaling->inRangeY(minY) || scaling->inRangeY(maxY) || inrangeY) {


			jsonObject shoulder_json;
			shoulder_json["ele"] = "line";
			shoulder_json["x1"] = x2Scaled;
			shoulder_json["x2"] = x2Scaled;
			shoulder_json["y1"] = scaling->scaleY(minY) + yshift;
			shoulder_json["y2"] = scaling->scaleY(maxY) + yshift;
			shoulder_json["stroke_width"] = branchWidth;
			shoulder_json["stroke"] = "black";
			shoulder_json["stroke_linecap"] = "round";
			objs.push_back(shoulder_json);

		}


	}


	// Branch
	if (!isRoot) {

		// Only draw if this node is in y-range
		if (inrangeY) {

			double x1 = this->getParent()->getHeight();

			// Branch
			jsonObject branch_json;
			branch_json["ele"] = "line";
			branch_json["x1"] = scaling->scaleX(x1);
			branch_json["x2"] = x2Scaled;
			branch_json["y1"] = yscaled;
			branch_json["y2"] = yscaled;
			branch_json["stroke_width"] = branchWidth;
			branch_json["stroke"] = "black";
			branch_json["stroke_linecap"] = "round";
			objs.push_back(branch_json);


		}


	}


	// Draw node and annotate it
	if (nodeRadius > 0 && inrangeY && (this->isLeaf() || nValidChildren > 1)) {
		jsonObject node_json;
		node_json["ele"] = "circle";
		node_json["cx"] = x2Scaled;
		node_json["cy"] = yscaled;
		node_json["r"] = nodeRadius;
		node_json["fill"] = "black";
		if (!this->isLeaf()) {
			node_json["class"] = "node";
			node_json["i"] = this->nodeNr;
		}
		if (!this->annotations.empty()) node_json["title"] = this->getTidyMetaData();
		objs.push_back(node_json);
	}



	// Internal/leaf node annotations?
	if (inrangeY && fontSize > 0) {

		string label = this->isLeaf() ? leafLabel : internalLabel;
		if (this->annotations.count(label) > 0) {


			string val = this->annotations[label];


			/*
			 * // Is it a double? If so then round it
			if (Utils::is_number(val)){
				double d = stod(val);

			}

			try {
			    double d = Double.parseDouble(val);
			    BigDecimal bd = new BigDecimal(d);
			    bd = bd.round(new MathContext(rounding));
			    val = "" + bd.doubleValue();
			}
			catch (Exception e) {};
			*/


			// Plot annotation
			jsonObject label_json;
			label_json["ele"] = "text";
			label_json["x"] = x2Scaled + nodeRadius;
			label_json["y"] = yscaled;
			label_json["text_anchor"] = "start";
			label_json["alignment_baseline"] = "middle";
			label_json["value"] = val;
			label_json["font_size"] = fontSize;
			objs.push_back(label_json);

		}


	}


	return y;


}


/*
 * Parse newick. Assumes there is no ; on the end
 */
void Node::parseFromNewick(string newick){

	this->annotations.clear();
	// Is this a leaf?
	char char1 = newick.at(0);
	bool hasChildren = char1 == '(';


	std::regex re("\\[.+");
	std::regex re1(",.*");
	std::regex re2("[(].*");
	std::regex re3("[)].*");



	// Get label
	//String[] labelSplit = labelAndAnnotations.split("\\[");
	//System.out.println(labelSplit[0] + " -> " + acc);
	if (!hasChildren) {

		// What is the node's name
		//String[] bits = newick.toString().split(":");
		vector<string> bits = Utils::split(newick, ":");
		if (bits.size()< 2 || bits.at(0).empty()) {
			Error::throwError("Cannot identify label for newick string. Please use : delimiter");
			bits.clear();
			return;
		}

		string acc = bits.at(0); //.replaceAll("([(]|\\[.+)", "");
		acc.erase(std::remove(acc.begin(), acc.end(), '('), acc.end());
		acc = regex_replace(acc, re, "$1");
		this->setAcc(acc);

	}





	// Find the first closing bracket on the same level
	char char2;
	vector<string> childSubtrees;// = new ArrayList<>();
	int level = 0;
	int pos = 1;
	int childPos = 1;
	int annotationLevel = 0;
	int annotationPos = -1;
	double length = 0;
	while (pos < newick.length()) {


		char2 = newick.at(pos);


		if (annotationLevel == 1 && char2 == ',') {

			// Parse annotation
			this->parseAnnotation(newick.substr(annotationPos, pos-annotationPos));
			annotationPos = pos+1;

		}

		// Parse node height
		else if (annotationLevel == 0 && level == 0 && char2 == ':') {
			string lengthStr = newick.substr(pos+1);
			lengthStr = regex_replace(lengthStr, re1, "$1");
			lengthStr = regex_replace(lengthStr, re2, "$1");
			lengthStr = regex_replace(lengthStr, re3, "$1");
			//.replaceAll("(,|[(]|[)]).*", "");
			length = stod(lengthStr);
			if (!hasChildren) break;
		}

		// Grandchild subtree
		else if (annotationLevel == 0 && char2 == '(') {
			level++;
		}

		// New child of this node
		else if (annotationLevel == 0 && level == 0 && char2 == ',') {
			childSubtrees.push_back(newick.substr(childPos, pos-childPos));
			childPos = pos+1;
		}

		// Child subtree has closed
		else if (annotationLevel == 0 && char2 == ')') {
			if (level == 0) {
				childSubtrees.push_back(newick.substr(childPos, pos+1-childPos));
				childPos = pos+1;
				break;
			}
			level--;
		}

		// Annotation
		else if (char2 == '[') {
			annotationLevel++;
			annotationPos = pos+1;
		}
		else if (char2 == ']') {
			if (annotationLevel == 1) this->parseAnnotation(newick.substr(annotationPos, pos-annotationPos));
			annotationLevel--;
		}

		// Array annotation
		else if (char2 == '{') {
			annotationLevel++;
		}
		else if (char2 == '}') {
			annotationLevel--;
		}

		pos ++;

	}



	// Validate
	if (hasChildren && childSubtrees.empty()) {
		Error::throwError("Could not parse newick. Perhaps there is a missing ')'.");
		return;
	}


	// Set node height
	if (!this->isRoot()) {
		this->setHeight(this->parent->getHeight() - length);
		//System.out.println(label + " has tree " + newick + " and height " + this.getHeight());
	}



	// Parse children
	for (string subtree : childSubtrees) {
		Node* child = new Node();
		this->addChild(child);
		//cout << "child newick: " << subtree << "\n\n" << endl;
		child->parseFromNewick(subtree);
	}



}



/*
 * Parse the annotation
 */
void Node::parseAnnotation(string annotation){

	int pos = annotation.find("=");
	string key = annotation.substr(0, pos);
	string val = annotation.substr(pos+1);
	key.erase(std::remove(key.begin(), key.end(), '&'), key.end());
	val.erase(std::remove(val.begin(), val.end(), '{'), val.end());
	val.erase(std::remove(val.begin(), val.end(), '}'), val.end());
	this->annotations[key] = val;
	//cout << key << " =" << val << endl;

}



/*
 * Return the height of the youngest child in the subtree
 */
double Node::getYoungestChildHeight(){
	double minHeight = this->getHeight();
	for (Node* child : this->getChildren()) {
		minHeight = std::min(minHeight, child->getYoungestChildHeight());
	}
	return minHeight;
}


/*
 * Populate the list with a unique list of annotations in the subtree
 */
void Node::getAllAnnotations(vector<string>& annots){
	for (const auto &pair : this->annotations) {
		string key = pair.first;
		if (std::count(annots.begin(), annots.end(), key) == 0) {
			annots.push_back(key);
		}
	}
	for (Node* child : this->getChildren()) {
		child->getAllAnnotations(annots);
	}
}


/*
 * Rotate the ordering of the children
 */
void Node::rotateChildren(){

	if (this->getChildCount() == 0) return;

	Node* lastChild = this->getChild(this->getChildCount()-1);
	this->children.insert(this->children.begin(), lastChild);
	this->children.erase(this->children.end()); // Remove last child

}


/*
 * Add case annotations
*/
void Node::addAnnotations(Case* c){
	for (string var : c->getVariables()) {
		string val = c->getValue(var);
		annotations[var] = val;
	}
}














