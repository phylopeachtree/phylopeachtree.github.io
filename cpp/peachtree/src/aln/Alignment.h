/*
 * Alignment.h
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */



#ifndef ALN_ALIGNMENT_H_
#define ALN_ALIGNMENT_H_



#include <vector>
#include <map>
#include <iostream>

#include "../lib/json.hpp"

#include "../options/Scaling.h"
#include "../phy/Tree.h"
#include "Sequence.h"
#include "Filtering.h"
#include "colourings/Colouring.h"


using namespace std;
using json = nlohmann::json;

class Epidemiology;
class Alignment {
public:

	Alignment(string& fasta);
	Alignment(Tree* tree);

	static int getNucleotideInt(string val);
	static int getAlphaInt(string val);
	static string getNucleotideStr(int index);
	static string getAlphaStr(int index);
	static bool isAmbiguous(int index, bool isNT);
	static bool isAmbiguousOrGap(int index, bool isNT);
	static bool isAmbiguousOrGap(string symbol, bool isNT);
	static void prepareAlignmentChars();
	static bool isGap(int index, bool isNT);
	static vector<char> getSymbols(bool isNT);
	


	Sequence* getSequence(int i);
	vector<Sequence*> getSequences();
	bool getIsNucleotide();
	int getLength();
	int getNtaxa();
	void cleanup();
	void annotateTaxa(Epidemiology* epi);
	void annotateTaxa(Tree* tree);
	string toFasta(Filtering* filtering, string annotation);
	string downloadSamples(Filtering* filtering, string sampleNameAnnotation, bool displayMissingPercentage, bool reportInfections);


	void sortByTree(Tree* tree);
	json toJSON();
	json getAlignmentGraphics(Scaling* scaling, Colouring* colouring, double minNtWidth, double textSize, Filtering* filtering, int siteNumberingEvery);
	json getTaxaGraphics(Scaling* scaling, double textSize, Filtering* filtering, bool showTaxonNumbers, bool displayMissingPercentage, string sampleNameAnnotation, bool reportInfections);

	vector<string> getNames();
	int getPatternCount();
	vector<char> getPattern(int patternNum);
	char getPattern(int patternNum, int taxonNum);
	int getPatternWeight(int i);
	bool selectTaxon(int taxonNum);
	bool taxonIsSelected(int taxonNum);
	void selectTaxon(int taxonNum, bool setTo);
	void clearSelection();
	void clearHighlighting();
	void recalculateTaxonIndices();

	Taxon* getTaxon(string label);
	vector<Taxon*> getTaxon(string value, string annotation);
	Taxon* getTaxon(int i);
	Taxon* getTaxonFromIndex(int index);
	int getTaxonIndex(Taxon* taxon);
	int getTaxonIndex(int taxonNum);
	
	bool isConstantSite(int siteNum);
	


private:


	void initPatterns();
	void initConstantSites();
	void parseSequence(string acc, string seq, int seqID);

	static std::map<string, int> nt_chars;
	static vector<string> nt_ids;
	static std::map<string, int> alpha_chars;
	static vector<string> alpha_ids;
	
	
	vector<bool> constantSites;

	vector<Sequence*> sequences;
	bool isNucleotide;
	int alignmentLength;
	vector<vector<char>> patterns;
	vector<int> patternWeights;

	static int ambiguousNtIndex;
	static int ambiguousAlphaIndex;
	static int gapNtIndex;
	static int gapAlphaIndex;

};

#endif /* ALN_ALIGNMENT_H_ */










