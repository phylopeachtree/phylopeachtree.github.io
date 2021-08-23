/*
 * Sequence.cpp
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */

#include "Sequence.h"
#include "Alignment.h"
#include "../Utils.h"
#include "../error/Error.h"

Sequence::Sequence(int id, string acc, string seq, bool couldBeNucleotide){

	this->taxon = new Taxon(id, acc);
	this->sequence = seq;

	// Is it nucleotide or amino acid?
	this->isNucleotide = true;
	seqLen = seq.length();
	if (couldBeNucleotide) this->prepareArray();
	
}










/*
 * Clear memory
 */
void Sequence::cleanup(){
	//this->sequenceArr.clear();
	delete taxon;
}

/**
 * Copy the sequence
 */
Sequence* Sequence::copy(){
	Sequence* seq = new Sequence(this->getTaxon()->getID(), this->getAcc(), this->sequence, this->isNucleotide);
	seq->setIsNucleotide(this->isNucleotide);
	if (!this->isNucleotide) seq->prepareArray();
	return seq;
}


/**
 * Prepare int array of the sequence
 */
void Sequence::prepareArray(){


	if (this->seqLen == 0) return;
	
	this->ungappedPos.clear();
	this->ungappedPos.resize(this->seqLen);
	int numNonGaps = 0;
	int numAmbiguousSites = 0;
	this->missingDataProportion = 0;
	

	//System.out.println("prepare array " + this.isNucleotide + " length " + this.sequence.length() + "/" + this.seqLen);
	//System.out.println("'" + this.sequence + "'");
	this->sequenceArr.resize(this->seqLen);
	for (int i = 0; i < this->seqLen; i ++) {
		string site = this->sequence.substr(i, 1);
		int val;
		
		if (this->isNucleotide) {
			val = Alignment::getNucleotideInt(site);
		}else {
			val = Alignment::getAlphaInt(site);
		}
		bool isGap = Alignment::isGap(val, this->isNucleotide);
		
		//System.out.println("putting " + val + " at site " + i + " for symbol " + site);
		if (val == -1) {
			string str1 = "Unknown character ";
			str1.append(site).append(" at site ").append(to_string(i+1)).append(" in ").append(this->getAcc());
			Error::throwError(str1);
			return;
		}
		this->sequenceArr.at(i) = val;
		
		
		// Count ungapped position
		if (isGap){
			this->ungappedPos.at(i) = -1;
		}else{
			numNonGaps ++;
			this->ungappedPos.at(i) = numNonGaps;
		}
		
		
		// Count missing
		if (Alignment::isAmbiguous(val, this->isNucleotide)) numAmbiguousSites ++;
		
	}
	
	
	this->missingDataProportion = 1.0 * numAmbiguousSites / numNonGaps;
	
}


double Sequence::getMissingDataProportion(){
	return this->missingDataProportion;
}

string Sequence::getSeq(){
	return this->sequence;
}


Taxon* Sequence::getTaxon(){
	return this->taxon;
}


string Sequence::getAcc(){
	return this->taxon->getName();
}


int Sequence::getID(){
	return this->taxon->getID();
}


json Sequence::toJSON(){
	json j;
	j["acc"] = this->getAcc();
	j["seq"] = this->sequence;
	return j;
}


int Sequence::getLength(){
	return this->seqLen;
}


bool Sequence::getIsNucleotide(){
	return this->isNucleotide;
}


void Sequence::setIsNucleotide(bool b){
	this->isNucleotide = b;
}


json Sequence::getTaxonGraphics(Scaling* scaling, int seqNum, Filtering* filtering, double textSize, bool showTaxonNumbers, double yshift, bool displayMissingPercentage, string sampleNameAnnotation){


	json arr; // = json::array();

	// Do not plot beyond the edge
	if (!scaling->inRangeY(seqNum) || textSize <= 0) return arr;


	// Y shift
	double yc_scaled = scaling->scaleY(seqNum + 0.5) + yshift;


	//cout << "y = " << yshift->at(0) << "/" << yc_scaled << "/" << seqNum << endl;

	string label = "";
	if (showTaxonNumbers){
		label.append(to_string(seqNum+1));
		label.append(": ");
	}
	if (sampleNameAnnotation != "" && sampleNameAnnotation != "None"){
		label.append(this->getTaxon()->getValue(sampleNameAnnotation));
	}else{
		label.append(this->getAcc());
	}
	
	
	//numberPadding.replaceAll(" ", "&#160;"); // White space


	string missingStr = "";
	string missingStrTitle = "";
	if (this->getLength() > 0) {
		string percentage;
		if (this->missingDataProportion == 0) percentage = "0";
		else{
			percentage = to_string(Utils::roundToSF(100*this->missingDataProportion, 2));
			percentage.erase(percentage.find_last_not_of('0') + 1, std::string::npos);
		}
		if (displayMissingPercentage) missingStr = " (" + percentage + "%)";
		missingStrTitle = "\n" + percentage + "% missing data";
	}

	// Plot accession
	json acc_json;
	acc_json["ele"] = "text";
	acc_json["x"] = scaling->scaleX(scaling->getXmin());
	acc_json["y"] = yc_scaled;
	acc_json["text_anchor"] = "start";
	acc_json["value"] = label + missingStr;
	acc_json["title"] = this->getAcc() + missingStrTitle;
	acc_json["font_size"] = textSize;
	acc_json["white_space"] = "pre";
	string str1 = "taxon";
	str1.append(this->getTaxon()->getIsHighlighted() ? " selected" : "");
	acc_json["class"] = str1;
	acc_json["i"] = seqNum;

	arr.push_back(acc_json);


	return arr;


}


/*
 * Get y top
 */
double Sequence::getY0(int seqNum, Scaling* scaling){
	double yc_rect_scaled = scaling->scaleY(seqNum);
	return scaling->getCanvasMinY() - yc_rect_scaled;
}


json Sequence::getSequenceGraphics(Scaling* scaling, int seqNum, double ntWidth, Colouring* colouring, Filtering* filtering, double textSize, double yshift){



	json arr = json::array();

	// Do not plot beyond the edge
	if (!scaling->inRangeY(seqNum)) return arr;


	double x = 0;

	double heightScaled = scaling->getRowHeight() + 1;
	//double x = scaling.xmin();


	// Y shift to avoid clipping the top margin
	double yc_rect_scaled = scaling->scaleY(seqNum);
	double yc_text_scaled = scaling->scaleY(seqNum + 0.5);
	yc_rect_scaled += yshift;
	yc_text_scaled += yshift;



	// Add some xshift to the first nucleotide so it doesn't get clipped by left boundary
	double xshift = Utils::INFTY; // -1 = null
	//double x0 = Utils::INFTY;
	//int siteNum = 0;

	// Plot sites
	json nt_bg;
	json nt_font;
	string colour;
	for (int site : filtering->getSites()) {

		// Do not plot beyond the edge
		if (!scaling->inRangeX(x, ntWidth)) {
			x ++;
			continue;
		}
		if (scaling->isAboveRangeX(x, ntWidth)) break;

		string symbol = this->getSymbol(site);





		double xc = scaling->scaleX(x);
		if (xshift == Utils::INFTY) {
			xshift = scaling->getCanvasMinX() - xc;
		}
		xc += xshift;


		/*
		double xc;
		if (x0 == Utils::INFTY) {
			double xc = scaling->scaleX(x);
			xshift = scaling->getCanvasMinX() - xc;
			xc += xshift;
			x0 = xc;
		}else{
			siteNum++;
			xc = x0 + ntWidth*siteNum;
		}
	*/



		// Background colour
		nt_bg["ele"] = "rect";
		nt_bg["x"] = xc;
		nt_bg["y"] = yc_rect_scaled;
		nt_bg["width"] = ntWidth+1;
		nt_bg["height"] = heightScaled;

		if (colouring != nullptr) {
			colour = colouring->getColour(symbol, site);
			nt_bg["fill"] = colour;
			nt_bg["stroke"] = colour;
		}
		//nt_bg.put("fill", "#008cba");
		//nt_bg.put("color", "white");



		arr.push_back(nt_bg);


		// Text
		if (textSize > 0) {
			xc = scaling->scaleX(x + 0.5) + xshift;
			//xc = xc + ntWidth/2.0;
			nt_font["ele"] = "text";
			nt_font["x"] = xc;
			nt_font["y"] = yc_text_scaled;
			nt_font["value"] = symbol;
			nt_font["text_anchor"] = "middle"; // Right alignment
			string str1 = "Site ";
			str1.append(to_string(site+1));
			///cout << site << endl;
			if (this->ungappedPos.at(site) != -1){
				str1 += "\nUngapped position: " + to_string(this->ungappedPos.at(site));
			}
			
			nt_font["title"] = str1;
			nt_font["font_size"] = textSize;
			arr.push_back(nt_font);
		}

		x ++;

	}




	return arr;


}


int Sequence::getSymbolInt(int site){
	return this->sequenceArr.at(site);
}


string Sequence::getSymbol(int site){

	int i = this->getSymbolInt(site);
	string val;
	if (this->isNucleotide) {
		val = Alignment::getNucleotideStr(i);
	}else {
		val = Alignment::getAlphaStr(i);
	}

	//cout << site << "->" << i << "->" << val << endl;

	//if (val == NULL) return "?";
	return val;
}


/**
 * Change the site
 */
void Sequence::editSiteInt(int siteNum, int newSymbol){
	this->sequenceArr.at(siteNum) = newSymbol;
}





