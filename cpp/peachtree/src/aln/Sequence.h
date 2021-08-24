/*
 * Sequence.h
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */






#ifndef ALN_SEQUENCE_H_
#define ALN_SEQUENCE_H_

#include <iostream>
#include <vector>
#include "Taxon.h"
#include "Filtering.h"
#include "../options/Scaling.h"
#include "colourings/Colouring.h"
#include "../lib/json.hpp"
using json = nlohmann::json;

using namespace std;


class Sequence {
public:
	Sequence(int id, string acc, string seq, bool couldBeNucleotide);


	Sequence* copy();
	void prepareArray();
	string getSeq();
	string getSeq(Filtering* filtering);
	Taxon* getTaxon();
	string getAcc();
	int getID();
	json toJSON();
	int getLength();
	bool getIsNucleotide();
	void setIsNucleotide(bool b);
	json getTaxonGraphics(Scaling* scaling, int seqNum, Filtering* filtering, double textSize, bool showTaxonNumbers, double yshift, bool displayMissingPercentage, string sampleNameAnnotation);
	json getSequenceGraphics(Scaling* scaling, int seqNum, double ntWidth, Colouring* colouring, Filtering* filtering, double textSize, double yshift);
	int getSymbolInt(int site);
	string getSymbol(int site);
	void editSiteInt(int siteNum, int newSymbol);
	double getY0(int seqNum, Scaling* scaling);
	void cleanup();
	double getMissingDataProportion();


private:

	Taxon* taxon;
	string sequence;
	vector<int> sequenceArr;
	int seqLen;
	bool isNucleotide;
	vector<int> ungappedPos;
	double missingDataProportion;

};

#endif /* ALN_SEQUENCE_H_ */








