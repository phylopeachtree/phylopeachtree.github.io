/*
 * Alignment.cpp
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */

#include "Alignment.h"
#include "../Utils.h"
#include <algorithm>
#include <locale>
#include "../error/Error.h"



std::map<string, int> Alignment::nt_chars;
vector<string> Alignment::nt_ids;
std::map<string, int> Alignment::alpha_chars;
vector<string> Alignment::alpha_ids;

int Alignment::ambiguousNtIndex;
int Alignment::ambiguousAlphaIndex;
int Alignment::gapNtIndex;
int Alignment::gapAlphaIndex;





Alignment::Alignment(string& fasta) {


	if (Alignment::nt_chars.size() == 0) this->prepareAlignmentChars();

	if (fasta.length() == 0) {
		Error::throwError("Empty file contents");
		return;
	}

	this->alignmentLength = -1;
	this->isNucleotide = true;


	// Parse fasta
	fasta.erase(std::remove(fasta.begin(), fasta.end(), '\r'), fasta.end());
	vector<string> lines = Utils::split(fasta, "\n");
	string acc = "";
	bool parsedSeq = true;
	int seqID = 0;

	string seq;
	const string opening = ">";
	for (string line : lines) {

		//cout << line << endl;

		if (line.substr(0, 1) == opening) {


			if (!parsedSeq) {
				Error::throwError("Cannot find sequence for " + acc);
				return;
			}

			// Parse the sequence
			if (seq.length() > 0) {

				parseSequence(acc, seq, seqID);
				seqID++;
				seq = "";
			}

			acc = line.substr(1);
			parsedSeq = false;

		}




		else {
			parsedSeq = true;
			seq.append(Utils::toUpper(line));
		}


	}


	// Parse the last sequence
	if (seq.length() > 0) {
		parseSequence(acc, seq, seqID);
	}



	if (this->sequences.empty()) {
		Error::throwError("Cannot detect any sequences in the alignment. Are you sure this is .fasta?");
		return;
	}



	// Rebuild all sequence arrays if it is not nucleotide
	if (!this->isNucleotide) {
		for (Sequence* sequence : sequences) {
			sequence->prepareArray();
		}
	}



	// Initialise patterns
	this->patterns.clear();
	//this.initPatterns();

	cout << "Parsed an alignment with " << this->alignmentLength << " sites and " << this->sequences.size() << " taxa " << endl;


}



/**
 * Prepare an empty alignment around tree sequences
 */
Alignment::Alignment(Tree* tree){

	this->alignmentLength = 0;
	this->isNucleotide = true;

	int i = 0;
	for (Node* leaf : tree->getLeavesAsArray()){
		Sequence* seq = new Sequence(i, leaf->getAcc(), "", true);
		leaf->setTaxon(seq->getTaxon());
		this->sequences.push_back(seq);
		i++;
	}

	cout << "Created a mock alignment with " << this->alignmentLength << " sites and " << this->sequences.size() << " taxa " << endl;

}





/*
 * Clear memory
 */
void Alignment::cleanup(){

	for (vector<int> patt : patterns){
		patt.clear();
	}
	this->patterns.clear();
	this->patternWeights.clear();

	for (Sequence* seq : this->sequences){
		seq->cleanup();
		delete seq;
	}
	this->sequences.clear();
}


/**
 * Initialise site patterns
 */
void Alignment::initPatterns(){


	this->patterns.clear();
	this->patternWeights.clear();


	vector<int> site(this->getNtaxa());
	for (int siteNum = 0; siteNum < this->getLength(); siteNum ++) {



		// Get pattern of this column
		for (int taxonNum = 0; taxonNum < site.size(); taxonNum++) {
			Sequence* sequence = this->getSequence(taxonNum);
			site[taxonNum] = sequence->getSymbolInt(siteNum);
		}


		// Check if it is unique
		int patternMatch = -1;
		for (int patternNum = 0; patternNum < this->patterns.size(); patternNum ++) {
			vector<int> pattern = this->patterns.at(patternNum);
			bool isUniqueFromThisPattern = false;
			for (int taxonNum = 0; taxonNum < site.size(); taxonNum++) {
				if (site.at(taxonNum) != pattern.at(taxonNum)) {
					isUniqueFromThisPattern = true;
					break;
				}
			}

			// This column is a duplicate. Do not add it to the list of unique patterns
			if (!isUniqueFromThisPattern) {
				patternMatch = patternNum;
				break;
			}

		}


		// If unique site, then add to list
		if (patternMatch == -1) {
			vector<int> siteCpy(site.size());
			for (int taxonNum = 0; taxonNum < site.size(); taxonNum++) {
				siteCpy[taxonNum] = site[taxonNum];
			}

			this->patterns.push_back(siteCpy);
			this->patternWeights.push_back(1.0);
		}

		// Otherwise increment the pattern weight
		else {
			double weight = this->patternWeights.at(patternMatch);
			this->patternWeights.at(patternMatch) = weight + 1.0;
		}

	}


	cout << "There are " << this->patterns.size() << " patterns" << endl;

}




/**
 * Add the sequence + accession
 */
void Alignment::parseSequence(string acc, string seq, int seqID){


	Sequence* sequence = new Sequence(seqID, acc, seq, this->isNucleotide);


	if (this->sequences.empty()) {
		this->alignmentLength = sequence->getLength();
		this->isNucleotide = sequence->getIsNucleotide();
	} else {

		// All sequences must have same length
		if (sequence->getLength() != alignmentLength) {
			string err = "Cannot parse sequence " + sequence->getAcc() + " because of length mismatch (" + to_string(sequence->getLength());
			err.append(" != ");
			err.append(to_string(alignmentLength));
			err.append(")");
			Error::throwError(err);
			return;
		}

		// Is it still a nucleotide alignment?
		if (this->isNucleotide && !sequence->getIsNucleotide()) {
			this->isNucleotide = false;
			for (Sequence* s : sequences) {
				s->setIsNucleotide(false);
			}
		}


		// Does it have a unique accession?
		for (Sequence* other : sequences) {
			if (other->getAcc() == sequence->getAcc()) {
				string err = "Duplicate sequence accession detected: ";
				err.append(sequence->getAcc());
				Error::throwError(err);
				return;
			}
		}

	}

	sequences.push_back(sequence);

}



/**
 * Sort the taxa by the tree and ensure the taxon objects are mapped
 */
void Alignment::sortByTree(Tree* tree){


	vector<Node*> leaves = tree->getLeavesAsArray();

	// Find a new ordering
	vector<int> newOrdering(leaves.size());
	for (int taxonNr = 0; taxonNr < leaves.size(); taxonNr ++) {

		string accession = leaves.at(taxonNr)->getAcc();

		// Find the sequence number with this accession
		int seqIndex = -1;
		for (int i = 0; i < this->getNtaxa(); i++) {
			Sequence* seq = this->getSequence(i);
			if (seq->getAcc() == accession) {
				leaves.at(taxonNr)->setTaxon(seq->getTaxon());
				seqIndex = i;
				break;
			}
		}
		if (seqIndex == -1) {
			Error::throwError("Cannot find " + accession + " in alignment");
			return;
		}

		newOrdering.at(taxonNr) = seqIndex;

	}


	// Create new sequence array
	vector<Sequence*> sequencesNew;


	// Reorder the array
	for (int newIndex = 0; newIndex < leaves.size(); newIndex ++) {
		int oldIndex = newOrdering.at(newIndex);
		sequencesNew.push_back(this->getSequence(oldIndex));
	}


	// Point to the new array
	this->sequences = sequencesNew;

	// Reinitialise
	this->patterns.clear();


}


/*
 * Get index of taxon
 */
int Alignment::getTaxonIndex(Taxon* taxon){

	for (int i = 0; i < this->sequences.size(); i++){
		Sequence* seq = this->sequences.at(i);
		if (seq->getTaxon() == taxon) return i;
	}
	return -1;

}


/*
 * Get JSON of alignment
 */
json Alignment::toJSON(){

	// Array of sequences
	json arr;
	for (int i = 0; i < this->sequences.size(); i ++) {
		Sequence* seq = this->sequences.at(i);
		arr.push_back(seq->toJSON());
	}


	// Alignment
	json j;
	j["alignment"] = arr;
	j["length"] = this->alignmentLength;
	j["ntaxa"] = this->sequences.size();
	j["isNT"] = this->isNucleotide;
	return j;

}

/**
 * Get graphics of the sequences in the alignment
 * @return
 */
json Alignment::getAlignmentGraphics(Scaling* scaling, Colouring* colouring, double minNtWidth, double textSize, Filtering* filtering, int siteNumberingEvery){

	json objs = json::array();
	if (!scaling->inView() || this->sequences.size() == 0) return objs;


	// Add some yshift to the first row so it doesn't get clipped by top margin
	double yshift = 0;
	int filteredNr = 0;
	for (int seqNum = 0; seqNum < this->sequences.size(); seqNum++) {
		Sequence* sequence = this->getSequence(seqNum);
		if (!filtering->includeTaxon(sequence->getTaxon())) continue;
		if (scaling->inRangeY(filteredNr)){
			yshift = sequence->getY0(filteredNr, scaling);
			break;
		}
		filteredNr++;

	}


	//cout << " yshift " << yshift << " / " << scaling->getCanvasMinY() << endl;


	// Site numbering at the top
	int siteLabelIndex = 0;
	int x = 0;
	double xshift = Utils::INFTY;

	// First pos
	for (int site : filtering->getSites()) {
		if (!scaling->inRangeX(x, minNtWidth)) {
			x++;
			continue;
		}else{
			xshift = scaling->getCanvasMinX() - scaling->scaleX(x);
			break;
		}
	}
	x=0;

	const double tickWidth = 2;
	const double tickHeight = textSize;
	for (int site : filtering->getSites()) {

		// Do not plot beyond edge
		if (!scaling->inRangeX(x, minNtWidth)) {
			x ++;
			continue;
		}


		int effSiteNum = filtering->getVariantSitesOnly() ? siteLabelIndex : site+1;

		// Plot the site number
		if (effSiteNum % siteNumberingEvery == 0){


			string label = to_string(site+1);

			// Is there enough space?
			int len = label.size();
			if (filtering->getVariantSitesOnly()) {
				label += "&#10203";
				len++;
			}
			if (!scaling->inRangeX(x+len, minNtWidth)) continue;



			double xc = scaling->scaleX(x) + xshift;


			// Draw a tick
			if (x > 0){
				json tick_json;
				tick_json["ele"] = "line";
				tick_json["x1"] = xc;
				tick_json["x2"] = xc;
				tick_json["y1"] = scaling->getCanvasMinY();
				tick_json["y2"] = scaling->getCanvasMinY()-tickHeight;
				tick_json["stroke_width"] = tickWidth;
				tick_json["stroke"] = "black";
				tick_json["stroke_linecap"] = "round";
				objs.push_back(tick_json);
			}


			// Plot the number
			json label_json;
			label_json["ele"] = "text";
			label_json["x"] = xc + tickWidth + 0.5;
			label_json["y"] = scaling->getCanvasMinY() - textSize/2.0;
			label_json["text_anchor"] = "start";
			//label_json["dominant_baseline"] = "hanging";
			label_json["value"] = label;
			label_json["font_size"] = textSize;
			objs.push_back(label_json);



		}



		siteLabelIndex++;
		x++;
		if (siteLabelIndex >= siteNumberingEvery) siteLabelIndex = 0;


	}



	int seqNumDisplayed = 0;
	for (int seqNum = 0; seqNum < this->sequences.size(); seqNum++) {
		if (scaling->isAboveRangeY(seqNumDisplayed)) break;
		Sequence* sequence = this->getSequence(seqNum);
		if (!filtering->includeTaxon(sequence->getTaxon())) continue;
		json j = sequence->getSequenceGraphics(scaling, seqNumDisplayed, minNtWidth, colouring, filtering, textSize, yshift);
		objs.insert(objs.end(), j.begin(), j.end()); // Add all
		seqNumDisplayed ++;
	}


	//delete yshift;


	return objs;

}


/**
 * Get taxa graphics
 * @return
 */
json Alignment::getTaxaGraphics(Scaling* scaling, double textSize, Filtering* filtering, bool showTaxonNumbers, bool displayMissingPercentage){


	json objs = json::array();
	if (!scaling->inView() || this->sequences.size() == 0) return objs;


	// Add some yshift to the first row so it doesn't get clipped by top margin
	double yshift = 0;
	int filteredNr = 0;
	for (int seqNum = 0; seqNum < this->sequences.size(); seqNum++) {
		Sequence* sequence = this->getSequence(seqNum);
		if (!filtering->includeTaxon(sequence->getTaxon())) continue;
		if (scaling->inRangeY(filteredNr)){
			yshift = sequence->getY0(filteredNr, scaling);
			break;
		}
		filteredNr++;

	}
	


	int seqNumDisplayed = 0;
	for (int seqNum = 0; seqNum < this->sequences.size(); seqNum++) {
		if (scaling->isAboveRangeY(seqNumDisplayed)) break;
		Sequence* sequence = this->getSequence(seqNum);
		if (!filtering->includeTaxon(sequence->getTaxon())) continue;
		json j = sequence->getTaxonGraphics(scaling, seqNumDisplayed, filtering, textSize, showTaxonNumbers, yshift, displayMissingPercentage);
		objs.insert(objs.end(), j.begin(), j.end()); // Add all
		seqNumDisplayed ++;
	}

	//delete yshift;

	return objs;

}


/**
 * Get a list of sequence accessions
 * @return
 */
vector<string> Alignment::getNames(){
	vector<string> names;
	for (Sequence* seq : this->sequences) {
		names.push_back(seq->getAcc());
	}
	return names;
}


/**
 * Number of site patterns
 */
int Alignment::getPatternCount(){
	if (this->patterns.size() == 0) this->initPatterns();
	return this->patterns.size();
}

/**
 * Gets the pattern index patternNum
 */
vector<int> Alignment::getPattern(int patternNum){
	if (this->patterns.size() == 0) this->initPatterns();
	return this->patterns.at(patternNum);
}

int Alignment::getPattern(int patternNum, int taxonNum){
	if (this->patterns.size() == 0) this->initPatterns();
	return this->patterns.at(patternNum).at(taxonNum);
}

/*
 * Weight of pattern i
*/
double Alignment::getPatternWeight(int i){
	if (this->patterns.size() == 0) this->initPatterns();
	return this->patternWeights.at(i);
}


/*
 * Toggle selection of this taxon
 */
bool Alignment::selectTaxon(int taxonNum){
	this->sequences.at(taxonNum)->getTaxon()->toggleSelection();
	this->sequences.at(taxonNum)->getTaxon()->toggleHighlighting();
	return this->sequences.at(taxonNum)->getTaxon()->getIsSelected();
}


/*
 * Check if the taxon is selected
 */
bool Alignment::taxonIsSelected(int taxonNum){
	return this->sequences.at(taxonNum)->getTaxon()->getIsSelected();
}


/*
 * Select/deselect taxon
 */
void Alignment::selectTaxon(int taxonNum, bool setTo){
	this->sequences.at(taxonNum)->getTaxon()->setIsSelected(setTo);
	if (setTo) this->sequences.at(taxonNum)->getTaxon()->setIsHighlighted(setTo);
}


/*
 * Deselct all taxa
 */
void Alignment::clearSelection(){
	for (Sequence* seq : this->sequences) {
		seq->getTaxon()->setIsSelected(false);
		seq->getTaxon()->setIsHighlighted(false);
	}
}


/*
 * Deselct all taxa
 */
void Alignment::clearHighlighting(){
	for (Sequence* seq : this->sequences) {
		seq->getTaxon()->setIsHighlighted(false);
	}
}


/*
 * Find the taxon with this label
 */
Taxon* Alignment::getTaxon(string label){
	for (Sequence* seq : this->getSequences()) {
		Taxon* taxon = seq->getTaxon();
		if (taxon->getName() == label) return taxon;
	}
	return nullptr;
}

Taxon* Alignment::getTaxon(int i){
	return this->sequences.at(i)->getTaxon();
}



/*
 * List of sequences in alignment
 */
vector<Sequence*> Alignment::getSequences(){
	return this->sequences;
}

Sequence* Alignment::getSequence(int i){
	return this->sequences.at(i);
}



/*
 * Get int from string
 */
int Alignment::getNucleotideInt(string val){
	if (Alignment::nt_chars.count(val) == 0) return -1;
	else return Alignment::nt_chars[val];
}


/*
 * Get int from string
 */
int Alignment::getAlphaInt(string val){
	if (Alignment::alpha_chars.count(val) == 0) return -1;
	else return Alignment::alpha_chars[val];
}

/*
 * Get string from int
 */
string Alignment::getNucleotideStr(int index){
	return Alignment::nt_ids.at(index);
}

/*
 * Get string from int
 */
string Alignment::getAlphaStr(int index){
	return Alignment::alpha_ids.at(index);
}


bool Alignment::getIsNucleotide(){
	return this->isNucleotide;
}


int Alignment::getLength() {
	if (this->sequences.empty()) return 0;
	return this->alignmentLength;
}


int Alignment::getNtaxa() {
	if (this->sequences.empty()) return 0;
	return this->sequences.size();
}



/*
 * Is the symbol int ambiguous
 */
bool Alignment::isAmbiguous(int index, bool isNT){

	if (isNT) {
		if (index > ambiguousNtIndex && index != gapNtIndex) return true;
	} else {
		if (index > ambiguousAlphaIndex && index != gapAlphaIndex) return true;
	}
	return false;

}



/*
 * Is the symbol int ambiguous/gap
 */
bool Alignment::isAmbiguousOrGap(int index, bool isNT){

	if (isNT) {
		if (index > ambiguousNtIndex) return true;
	} else {
		if (index > ambiguousAlphaIndex) return true;
	}
	return false;

}


/*
 * Is the symbol int ambiguous or a gap?
 */
bool Alignment::isAmbiguousOrGap(string symbol, bool isNT){

	if (isNT) {
		int index = Alignment::getNucleotideInt(symbol);
		if (index == -1) return true;
		if (index > ambiguousNtIndex) return true;
	}else {
		int index = Alignment::getAlphaInt(symbol);
		if (index == -1) return true;
		if (index > ambiguousAlphaIndex) return true;
	}

	return false;
}


/*
 * Is the symbol int  a gap?
 */
bool Alignment::isGap(int index, bool isNT){

	if (isNT) {
		if (index == Alignment::gapNtIndex) return true;
	}else {
		if (index == Alignment::gapAlphaIndex) return true;
	}
	return false;
}


/**
 * Initialise maps of nucleotides and alphas
 */
void Alignment::prepareAlignmentChars(){

	cout << "initialising alignment symbols" << endl;



	Alignment::ambiguousNtIndex = -1;
	Alignment::ambiguousAlphaIndex = -1;
	Alignment::gapNtIndex = -1;
	Alignment::gapAlphaIndex = -1;
	Alignment::nt_chars.clear();
	Alignment::alpha_chars.clear();


	// Nucleotide
	int i = -1;

	Alignment::nt_chars["A"] = ++i;
	Alignment::nt_chars["C"] = ++i;
	Alignment::nt_chars["G"] = ++i;
	Alignment::nt_chars["T"] = ++i;
	Alignment::nt_chars["U"] = ++i;


	// Ambiguous nucleotides / gaps
	Alignment::ambiguousNtIndex = i;
	Alignment::nt_chars["R"] = ++i; // Purine (AG)
	Alignment::nt_chars["Y"] = ++i; // Pyrimidine (TC)
	Alignment::nt_chars["K"] = ++i; // Keto (GT)
	Alignment::nt_chars["M"] = ++i; // Amino (AC)
	Alignment::nt_chars["S"] = ++i; // Strong (GC)
	Alignment::nt_chars["W"] = ++i; // Weak (AT)
	Alignment::nt_chars["B"] = ++i; // not A
	Alignment::nt_chars["D"] = ++i; // not C
	Alignment::nt_chars["H"] = ++i; // not G
	Alignment::nt_chars["V"] = ++i; // not T/U


	Alignment::nt_chars["N"] = ++i; // Ambiguous
	Alignment::nt_chars["-"] = ++i;
	Alignment::gapNtIndex = i;



	// Reverse mapping
	Alignment::nt_ids.resize(Alignment::nt_chars.size());
	for (const auto &pair : Alignment::nt_chars) {
		string key = pair.first;
		int index = Alignment::nt_chars[key];
		//cout << key << "->" << index << endl;
		Alignment::nt_ids.at(index) = key;
	}




	// General case (including aa)
	i = -1;
	Alignment::alpha_chars["A"] = ++i;
	Alignment::alpha_chars["C"] = ++i;
	Alignment::alpha_chars["D"] = ++i;
	Alignment::alpha_chars["E"] = ++i;
	Alignment::alpha_chars["F"] = ++i;
	Alignment::alpha_chars["G"] = ++i;
	Alignment::alpha_chars["H"] = ++i;
	Alignment::alpha_chars["I"] = ++i;
	Alignment::alpha_chars["K"] = ++i;
	Alignment::alpha_chars["L"] = ++i;
	Alignment::alpha_chars["M"] = ++i;
	Alignment::alpha_chars["N"] = ++i;
	Alignment::alpha_chars["P"] = ++i;
	Alignment::alpha_chars["Q"] = ++i;
	Alignment::alpha_chars["R"] = ++i;
	Alignment::alpha_chars["S"] = ++i;
	Alignment::alpha_chars["T"] = ++i;
	Alignment::alpha_chars["V"] = ++i;
	Alignment::alpha_chars["W"] = ++i;
	Alignment::alpha_chars["Y"] = ++i;

	// Ambiguous aas / gaps
	Alignment::ambiguousAlphaIndex = i;
	Alignment::alpha_chars["B"] = ++i;
	Alignment::alpha_chars["J"] = ++i;
	Alignment::alpha_chars["O"] = ++i;
	Alignment::alpha_chars["U"] = ++i;
	Alignment::alpha_chars["X"] = ++i;
	Alignment::alpha_chars["Z"] = ++i;
	Alignment::alpha_chars["-"] = ++i;
	Alignment::gapAlphaIndex = i;


	// Reverse mapping
	Alignment::alpha_ids.resize(Alignment::alpha_chars.size());
	for (const auto &pair : Alignment::alpha_chars) {
		string key = pair.first;
		int index = Alignment::alpha_chars[key];
		//cout << key << "->" << index << endl;
		Alignment::alpha_ids.at(index) = key;

	}



}



