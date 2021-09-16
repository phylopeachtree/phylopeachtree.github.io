/*
 * Node.cpp
 *
 *  Created on: 6/07/2021
 *      Author: jdou557
 */



#include "Node.h"
#include "../Utils.h"
#include "../error/Error.h"
#include "../epi/Timeline.h"
#include "../epi/Case.h"


Node::Node(){
	this->acc = "";
	this->nodeNr = 0;
	this->filteredNodeNr = 0;
	this->height = 0;
	this->parent = nullptr;
	this->taxon = nullptr;
	this->sampleTime = Utils::INFTY;
	this->isCompatibleTransmissionEvent = true;
}


Node::Node(int index){
	this->acc = "";
	this->nodeNr = index;
	this->filteredNodeNr = 0;
	this->height = 0;
	this->parent = nullptr;
	this->taxon = nullptr;
	this->sampleTime = Utils::INFTY;
	this->isCompatibleTransmissionEvent = true;
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

void Node::setFilteredNr(int nr){
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


/*
 * Node height as adjusted by timeline sample dates
 */
double Node::getHeight(Timeline* timeline){
	if (timeline == nullptr || !timeline->isReady()) return this->getHeight();
	return timeline->getSampleHeight(this);
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
 * Return the height of the youngest child in the subtree, with node heights informed by sample dates
 */
double Node::getYoungestChildHeight(Timeline* timeline){
	if (timeline == nullptr || !timeline->isReady()) return this->getYoungestChildHeight();
	double minHeight = this->getHeight(timeline);
	for (Node* child : this->getChildren()) {
		minHeight = std::min(minHeight, child->getYoungestChildHeight(timeline));
	}
	return minHeight;
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
	

	clone->filteredNodeNr = this->filteredNodeNr;
	clone->acc = this->acc;
	clone->height = this->height;
	clone->sampleTime = this->sampleTime;
	clone->isCompatibleTransmissionEvent = this->isCompatibleTransmissionEvent;
	for (const auto &pair : this->annotations) {
		string key = pair.first;
		clone->annotations[key] = this->annotations[key];
	}
	for (Node* child : children) {
		clone->addChild(child->copy());
	}
	clone->taxon = this->taxon;
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
	int dummy = -1;
	vector<string>* labels = new vector<string>();
	string str = this->toSortedNewick(dummy, false, labels);
	delete labels;
	return str;
}

/*
 * Print node as a newick
 * Adapted from beast2.evolution.tree.Node
 */
string Node::toSortedNewick(int& maxNodeInClade, bool printMetaData, vector<string>* labels){
	string buf;
	
	bool isStart = false;
	if (maxNodeInClade == -1) {
		maxNodeInClade = 0;
		isStart = true;
	}


    if (!isLeaf()) {

        if (this->getChildCount() <= 2) {
            // Computationally cheap method for special case of <=2 children

            buf.append("(");
            string child1 = getChild(0)->toSortedNewick(maxNodeInClade, printMetaData, labels);
            int child1Index = maxNodeInClade;
            if (getChildCount() > 1) {
                string child2 = getChild(1)->toSortedNewick(maxNodeInClade, printMetaData, labels);
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
                childStrings.at(i) = getChild(i)->toSortedNewick(maxNodeInClade, printMetaData, labels);
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
        //maxNodeInClade = nodeNr;
		if (labels != nullptr) labels->push_back(this->getAcc());
        //buf.append(nodeNr + 1);
    }

	buf.append(to_string(maxNodeInClade));
	maxNodeInClade++;
	
   // buf.append(this->getAcc());

    if (printMetaData) {
        buf.append(getNewickMetaData());
    }
    buf.append(":");

    //if (printMetaData) buf.append(getNewickLengthMetaData());
    buf.append(to_string(getLength()));
	
	
	// Translate
	if (isStart && labels != nullptr){
		
		string header = "#NEXUS\n";
		header += "Begin taxa;\n";
		header += "\tDimensions ntax=" + to_string(this->getLeafNodeCount()) + ";\n";
		header += "\t\tTaxlabels\n";
		for (int i = 0; i < labels->size(); i++){
			header += "\t\t\t" + labels->at(i) + "\n";
		}
		header += "\t\t\t;\n";
		
		header += "End;\n";
		header += "Begin trees;\n";
		header += "\tTranslate\n";
		for (int i = 0; i < labels->size(); i++){
			header += "\t\t" + to_string(i+1) + " " + labels->at(i);
			if (i < labels->size()-1) header += ",";
			header += "\n";
		}
		header += "\t\t;\n";
		
		
		header += "tree TREE1 = " +  buf + ";\n";
		header += "End;";
		buf = header;


		
		
	}
	

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
string Node::getTidyMetaData(Timeline* timeline){


	string str;
	if (this->isLeaf()) {
		str.append(this->getAcc());
	}
	else {
		str.append("Click node to switch child ordering.");
	}
	str.append("\nheight=");
	str.append(to_string(this->getHeight(timeline))); //OptionsAPI.sf(this->getHeight()));
	if (!this->isRoot()){
		str.append("\nlength=");
		str.append(to_string(this->getParent()->getHeight(timeline) - this->getHeight(timeline))); //OptionsAPI.sf(this->getHeight()));
	}

	if (!this->annotations.empty()) str.append("\n");
	
	int i = 0;
	for (const auto & pair: this->annotations) {
		string key = pair.first;
		string value = pair.second;


		// Rounding
		if (Utils::is_number(value)) {
			double val = stof(value);
			value = to_string(Utils::roundToSF(val, 4));
		}


		str.append(key).append("=").append(value);
		if (i < this->annotations.size()-1) str.append("\n");
		i++;
	}
	
	
	// epi annotations
	i = 0;
	if (this->getTaxon() != nullptr && this->getTaxon()->getCase() != nullptr){
		
		str.append("\n\nEpidemiology:\n");
		
		Case* c = this->getTaxon()->getCase();
		for (string var : c->getVariables()) {
			string val = c->getValue(var);
		
			str.append(var).append("=").append(val);
			if (i < c->getVariables().size()-1) str.append("\n");
			i++;
		
		}
		
		
	}
	
	

	return str;

}

/*
 * Get annotation value
 */
string Node::getAnnotationValue(string var){
	if (this->annotations.find(var) != this->annotations.end()) return annotations[var];
	
	if (this->getTaxon() != nullptr && this->getTaxon()->getCase() != nullptr){
		Case* c = this->getTaxon()->getCase();
		return c->getValue(var);
	}
	
	
	return "";
	
}


/**
 * Get hex code for this value
 */
string Node::getAnnotationColour(double val, double min, double max, string colourMax){


	// Get RGB
	int red = Utils::getRed(colourMax);
	int green = Utils::getGreen(colourMax);
	int blue = Utils::getBlue(colourMax);


	// Invert the colour
	int red_ = 255 - red;
	int green_ = 255 - green;
	int blue_ = 255 - blue;


	// Scale rgb
	double scale = (val - min) / (max - min);
	if (scale < 0) scale = 0;
	if (scale > 1) scale = 1;
	int redVal = (red - red_)*scale + red_;
	int greenVal = (green - green_)*scale + green_;
	int blueVal = (blue - blue_)*scale + blue_;


	//cout << scale << ":" << red << "|" << green << "|" << blue << " | " << redVal << "|" << greenVal << "|" << blueVal << endl;


	// Convert to hex
	string colourHex = Utils::getHexCode(redVal, greenVal, blueVal);

	//cout << "colourHex: " << colourHex << endl;


	return colourHex;

}


/**
 * Get hex code for this annotation
 */
string Node::getAnnotationColour(string var, double min, double max, string colourMax){

	string val = this->getAnnotationValue(var);
	//cout << var << "=" << val << "|" << min << "," << max << endl;
	if (val == "") return "#000000";
	double val_d = 0;
	if (Utils::is_number(val)) val_d = stof(val);

	return this->getAnnotationColour(val_d, min, max, colourMax);
}


/*
 * Get the svg graphics of this subtree
 */
double Node::getGraphics(bool isRoot, jsonObject& objs, Filtering* filtering, Scaling* scaling, double branchWidth,
		bool showTaxaOnTree, double yshift, double nodeRadius, string branchColourBy, string nodeColourBy, double fontSize, int rounding,
		bool transmissionTree, Timeline* timeline, bool displayIncompatibleTransmissions, string branchCol, string nodeCol,
		vector<double>& minMaxNode, vector<double>& minMaxBranch){


	double x2 = this->getHeight(timeline);


	double x2Scaled = scaling->scaleX(x2);
	double maxY = -Utils::INFTY;
	double minY =  Utils::INFTY;

	// If leaf, go with node ordering
	double y = Utils::INFTY;
	int nValidChildren = 0;
	if (this->isLeaf()) {
		//if (!filtering->includeTaxon(this->getTaxon())) return Utils::INFTY;
		y = this->getFilteredNr() + 0.5;
		if (!filtering->includeTaxon(this->getTaxon())) return y;
	}


	// If parent node, y is midpoint between children
	else {

		for (Node* child : this->getChildren()) {
			double ychild = child->getGraphics(false, objs, filtering, scaling, branchWidth, showTaxaOnTree, yshift,
												nodeRadius, branchColourBy, nodeColourBy, fontSize, rounding, transmissionTree,
												timeline, displayIncompatibleTransmissions, branchCol, nodeCol, minMaxNode, minMaxBranch);
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
	bool shoulderInRange = scaling->isLineInRangeY(minY, maxY);


	// Colours
	string bcol = branchCol;
	string ncol = nodeCol;
	if (branchColourBy != "") bcol = getAnnotationColour(branchColourBy, minMaxBranch.at(0), minMaxBranch.at(1), branchCol);
	if (nodeColourBy != "") ncol = getAnnotationColour(nodeColourBy, minMaxNode.at(0), minMaxNode.at(1), nodeCol);

	// Dashed line to taxa
	if (this->isLeaf() && showTaxaOnTree && this->getHeight(timeline) > 0) {

		// Only draw if this node is in y-range
		if (inrangeY) {

			jsonObject dashed_json;
			dashed_json["ele"] = "line";
			dashed_json["x1"] = scaling->getCanvasMaxX();
			dashed_json["x2"] = x2Scaled;
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
	if (shoulderInRange && !this->isLeaf() && nValidChildren > 1) {


		jsonObject shoulder_json;
		shoulder_json["ele"] = "line";
		shoulder_json["x1"] = x2Scaled;
		shoulder_json["x2"] = x2Scaled;
		shoulder_json["y1"] = scaling->scaleY(minY) + yshift;
		shoulder_json["y2"] = scaling->scaleY(maxY) + yshift;
		shoulder_json["stroke_width"] = branchWidth;
		shoulder_json["stroke"] = bcol;
		shoulder_json["stroke_linecap"] = "round";
		objs.push_back(shoulder_json);



	}


	// Branch
	if (!isRoot) {

		// Only draw if this node is in y-range
		if (inrangeY) {

			double x1 = this->getParent()->getHeight(timeline);

			// Branch
			jsonObject branch_json;
			branch_json["ele"] = "line";
			branch_json["x1"] = scaling->scaleX(x1);
			branch_json["x2"] = x2Scaled;
			branch_json["y1"] = yscaled;
			branch_json["y2"] = yscaled;
			branch_json["stroke_width"] = branchWidth;
			branch_json["stroke"] = bcol;
			branch_json["stroke_linecap"] = "round";
			objs.push_back(branch_json);

		}

	}



	// Draw node and annotate it
	bool drawNode =  nodeRadius > 0 && inrangeY && (this->isLeaf() || nValidChildren > 1);
	

	if (drawNode) {


		
		jsonObject node_json;
		node_json["ele"] = "circle";
		node_json["cx"] = x2Scaled;
		node_json["cy"] = yscaled;
		node_json["r"] = nodeRadius;
		node_json["fill"] = ncol;
		node_json["stroke"] = "black";
		node_json["layer"] = 2;
		if (!this->isLeaf()) {
			node_json["class"] = "node";
			node_json["i"] = this->nodeNr;
		}
		string title = this->getTidyMetaData(timeline);



		// Compatible?
		if (displayIncompatibleTransmissions && !this->isCompatibleTransmissionEvent && timeline != nullptr && timeline->isReady()){

			double const tickLen = nodeRadius*1.618;

			// Top-left to bottom-right
			jsonObject tick1;
			tick1["ele"] = "line";
			tick1["x1"] = x2Scaled - tickLen;
			tick1["x2"] = x2Scaled + tickLen;
			tick1["y1"] = yscaled - tickLen;
			tick1["y2"] = yscaled + tickLen;
			tick1["stroke_width"] = branchWidth;
			tick1["stroke"] = "red";
			tick1["stroke_linecap"] = "round";
			tick1["layer"] = 2;
			//tick1["title"] = title;
			objs.push_back(tick1);


			// Bottom-left to top-right
			jsonObject tick2;
			tick2["ele"] = "line";
			tick2["x1"] = x2Scaled - tickLen;
			tick2["x2"] = x2Scaled + tickLen;
			tick2["y1"] = yscaled + tickLen;
			tick2["y2"] = yscaled - tickLen;
			tick2["stroke_width"] = branchWidth;
			tick2["stroke"] = "red";
			tick2["stroke_linecap"] = "round";
			tick2["layer"] = 2;
			//tick2["title"] = title;
			objs.push_back(tick2);

			if (this->getChildCount() > 0){
				tm transmissionDate = timeline->getSampleDateOfCase(this);

				// Get leaf
				Node* descendant = this->getChild(0);
				//cout << " child of " << this->getAcc() << " is " << descendant->getAcc() << endl;
				while (!descendant->isLeaf()) {
					string a = descendant->getAcc();
					descendant = descendant->getChild(0);
					
				}

				title = title + "\n\n" + descendant->getAcc() + " could not have caused this transmission event because they were not infectious on " + Utils::formatDate(transmissionDate);
			}

		}

		node_json["title"] = title;
		if (nodeColourBy == "" || this->getAnnotationValue(nodeColourBy) != "") objs.push_back(node_json);

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
		size_t found = acc.find_first_of("[");
		if (found != -1) acc = acc.substr(0, found);
		//acc = regex_replace(acc, re, "$1");
		this->setAcc(acc);

	}



	// Find the first closing bracket on the same level
	char char2;
	vector<string> childSubtrees;// = new ArrayList<>();
	int level = 0;
	long pos = 1;
	long childPos = 1;
	int annotationLevel = 0;
	int annotationPos = -1;
	double length = 0;
	while (pos < newick.length()) {


		char2 = newick.at(pos);


		if (annotationLevel == 1 && char2 == ',') {



			// Parse annotation
			if (!hasChildren || level == -1) {
				this->parseAnnotation(newick.substr(annotationPos, pos-annotationPos));
				//break;
			}
			annotationPos = pos+1;
			
			

		}

		// Parse node height
		else if (annotationLevel == 0 && level == 0 && char2 == ':') {
			string lengthStr = newick.substr(pos+1);

			size_t found = lengthStr.find_first_of(":");
			if (found != -1 && found < lengthStr.size()-1) lengthStr = lengthStr.substr(found+1);
			
			
			found = lengthStr.find_first_of("(");
			if (found != -1) lengthStr = lengthStr.substr(0, found);
			
			
			found = lengthStr.find_first_of(")");
			if (found != -1) lengthStr = lengthStr.substr(0, found);
			
			

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
				//break;
			}
			level--;
		}

		// Annotation
		else if (char2 == '[') {
			annotationLevel++;
			annotationPos = pos+1;
		}
		else if (char2 == ']') {
			if ((/*!hasChildren ||*/ level == -1) && annotationLevel == 1) {
				this->parseAnnotation(newick.substr(annotationPos, pos-annotationPos));
				break;
			}
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
		Error::throwError("Could not parse newick. Perhaps there is a missing ).");
		return;
	}


	// Set node height
	if (!this->isRoot()){
		this->setHeight(this->parent->getHeight() - length);
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
	//val.erase(std::remove(val.begin(), val.end(), '{'), val.end());
	//val.erase(std::remove(val.begin(), val.end(), '}'), val.end());
	this->annotations[key] = val;
	

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
	/*
	vector<int> filterNums;
	for (int i = 0; i < this->getChildCount(); i++){
		filterNums.push_back(this->getChild(i)->getFilteredNr());
	}
	*/

	Node* lastChild = this->getChild(this->getChildCount()-1);
	this->children.insert(this->children.begin(), lastChild);
	this->children.erase(this->children.end()); // Remove last child


	/*
	int lastNr = filterNums.at(filterNums.size() - 1);
	filterNums.insert(filterNums.begin(), lastNr);
	filterNums.erase(filterNums.end()); // Remove last filter nr
	for (int i = 0; i < this->getChildCount(); i++){
		Node* child = this->children.at(i);
		child->setFilteredNr(filterNums.at(i));
	}
	*/

}


/*
 * Add case annotations
*/
void Node::addAnnotations(Case* c){
	for (string var : c->getVariables()) {
		string val = c->getValue(var);
		annotations[var] = val;
		//cout << "setting node " << var << " = " << val << endl;
	}
}



/*
 * Is this compatible?
 */
bool Node::getIsCompatibleTransmissionEvent(){
	return this->isCompatibleTransmissionEvent;
}


/*
 * Set compatibity
 */
void Node::setIsCompatibleTransmissionEvent(bool val){
	this->isCompatibleTransmissionEvent = val;
}

/*
 * Get the sample time
 */
double Node::getSampleTime(){
	return this->sampleTime;
}


/*
 * Set the sample time
 */
void Node::setSampleTime(double time){
	this->sampleTime = time;
}


/*
 * Clear the sample time
 */
void Node::clearSampleTime(){
	this->sampleTime = Utils::INFTY;
}

/*
 * Is there a sample time?
 */
bool Node::hasSampleTime(){
	return this->sampleTime != Utils::INFTY;
}


/*
 * Get min and max value
 */
void Node::getMinMax(string var, vector<double>& minMax){


	if (var == "") return;
	string val = this->getAnnotationValue(var);
	if (val != "") {
	double val_d = 0;
	if (Utils::is_number(val)) val_d = stof(val);

	if (val_d < minMax.at(0)) minMax.at(0) = val_d;
	if (val_d > minMax.at(1)) minMax.at(1) = val_d;

	}
	for (Node* child : this->getChildren()){
		child->getMinMax(var, minMax);
	}

}


bool sortNodeHeight(pair<Node*, double> a, pair<Node*, double> b) { 
	return (a.second < b.second);
}


/*
 * Get the 1st child's 1st childs' ... leaf
 */
Node* Node::getFirstLeaf(){
	if (this->isLeaf()) return this;
	return this->children.at(0)->getFirstLeaf();
}

/*
 * Reorder nodes in the transmission tree to maximise compatibility with infectious periods
 * Post-order traversal: sort children in chronological order of symptom onset date
 */
void Node::reorderTransmissions(Timeline* timeline, string symptomDateVar){
	
	if (timeline == nullptr) return;
	
	// Np need to make changes if this is a leaf
	if (this->isLeaf()) return;
	
	// Do children first
	for (Node* child : this->getChildren()){
		child->reorderTransmissions(timeline, symptomDateVar);
	}
	
	// Sort children by symptom onset date
	vector<pair<Node*, double>> nodeSymptomTimes;
	for (Node* child : this->getChildren()){
		Node* leaf = child->getFirstLeaf();
		string val = leaf->getAnnotationValue(symptomDateVar);
		double height = timeline->getDateAsTime(val);
		pair<Node*, double> node_height{child, height};
		nodeSymptomTimes.push_back(node_height);
	}
	std::sort (nodeSymptomTimes.begin(), nodeSymptomTimes.end(), sortNodeHeight);  
	
	
	// Reorder children
	vector<Node*> newChildren;
	for (pair<Node*, double>& x : nodeSymptomTimes){
		newChildren.push_back(x.first);
	}
	this->children = newChildren;
	


	
}



/*
 * Count the number of infections cased by each case
 */
void Node::countInfections(){
	
	if (this->isLeaf()){
		
		// Count the number of internal nodes going back until this leaf was infected
		int numInfections = 0;
		Node* parent = this->getParent();
		Node* child = this;
		while (parent != nullptr){
			
			
			if (parent->getChild(0) == child){
				numInfections++;
			}else{
				break;
			}
			
			child = parent;
			parent = parent->getParent();
		}
		
		//cout << "numInfections " << taxon->getName() << ": " << numInfections << " " << this->taxon->getInfectionCount() << endl;
		this->taxon->setInfectionCount(this->taxon->getInfectionCount() + numInfections);

	}else{
		for (Node* child : this->getChildren()){
			child->countInfections();
		}
	}
	
	
}



/*
 * Set num infections of each case to 0
 */
void Node::resetInfections(){
	if (this->isLeaf()){
		this->taxon->setInfectionCount(0);
	}else{
		for (Node* child : this->getChildren()){
			child->resetInfections();
		}
	}
}
/*
 * Divide num infections of each case by 'numTrees'
 */
void Node::normaliseInfections(int numTrees){
	if (this->isLeaf()){
		double newCount = 1.0 * this->taxon->getInfectionCount() / numTrees;
		cout << this->getTaxon()->getName() << " : " << this->taxon->getInfectionCount() << "/" << numTrees << " = " << newCount << endl;
		this->taxon->setInfectionCount(newCount);
	}else{
		for (Node* child : this->getChildren()){
			child->normaliseInfections(numTrees);
		}
	}
}




/*
 * Build a subtree containing only the mentioned taxa
 */
bool Node::getSubtree(vector<Taxon*> taxa){
	
	if (this->isLeaf()){
		
		
		// Return true if this leaf should be included
		for (Taxon* taxon : taxa){
			if (this->getTaxon()->getID() == taxon->getID()) return true;
		}
		
		return false;
		
	}else{
		
		// Return true if this node has leaves that should be included
		vector<Node*> newChildren;
		for (Node* child : this->getChildren()){
			
			if (child->getSubtree(taxa)){
				newChildren.push_back(child);
			}else{
				child->cleanup();
				delete child;
			}
			
		}
		
		
		
		// Keep this node
		if (this->children.size() == newChildren.size()){
			return true;
		}
		
		this->children.clear();
		for (Node* child : newChildren){
			this->addChild(child);
		}
		

		// Delete this subtree
		if (newChildren.size() == 0){
			return false;
		}
		
		
		// Keep
		return true;

		
	}
		
		
	
}




