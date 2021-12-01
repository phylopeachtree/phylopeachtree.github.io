/*
 * Filtering.h
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */






#ifndef ALN_FILTERING_H_
#define ALN_FILTERING_H_

class Alignment;
class Taxon;

#include <iostream>
#include <vector>
#include <map>




using namespace std;

class Tree;
class Node;

class Filtering {


public:

	Filtering(bool variantSitesOnly, bool focus, bool focusOnClade, Alignment* alignment,  Tree* tree);
	void init(bool variantSitesOnly, bool focus, bool focusOnClade, Alignment* alignment, Tree* tree);

	void updateOrdering(Alignment* alignment);

	bool includeTaxon(Taxon* taxon);
	Node* getSubtreeRoot();
	bool getVariantSitesOnly();
	bool variantSitesOnlyParsed();
	Tree* getTree();
	bool getFocusing();
	bool getFocusingOnClade();
	bool includeSite(int site);
	vector<int> getSites();
	int getNumSites();
	int getNumSeqs();
	bool isMajorAllele(string character, int siteNum);
	bool isMinorAllele(string character, int siteNum);
	int getIndex(Taxon* taxon);
	int getNumUniqueSequences();
	void prepareMajorAlleles(Alignment* alignment);
	void cleanup();
	int getTaxonIDByIndex(int index);
	int getIndexByTaxonID(int taxonID);

private:

	bool isMajorOrMinorAllele(string character, int siteNum, bool isMajor);


	bool variantSitesOnly;
	bool variantSitesOnly_parsed; // Value parsed in construction
	bool focusing;
	bool focusingOnClade;
	std::map<int, bool> taxaIDsToInclude;
	std::map<int, bool> sitesToIncludeMap;
	vector<int> sitesToIncludeList;
	vector<int> taxonIDsIncludeList;
	Alignment* alignment;
	int numUniqueSeqs;

	// Major characters at each site
	std::map<int, int> majors;

	int numSites;
	int numTaxa;
	bool isNucleotide;

	Tree* tree;
	Node* subtree; // The subtree (if using a tree as a filter)

};

#endif /* ALN_FILTERING_H_ */




