/*
 * AlignmentAPI.h
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */




#ifndef ALN_ALIGNMENTAPI_H_
#define ALN_ALIGNMENTAPI_H_

#include <iostream>
#include "../aln/Alignment.h"
#include "../aln/Filtering.h"
#include "../phy/Tree.h"
#include "../lib/json.hpp"
using namespace std;
using jsonObject = nlohmann::json;


class AlignmentAPI {

public:

	static jsonObject getTaxaGraphics(Scaling* scaling, double textSize, bool showTaxonNumbers);
	static jsonObject getAlignmentGraphics(Scaling* scaling, double minNtWidth, double textSize, Colouring* colouring);


	static bool isReady();
	static void initFiltering(bool variantSitesOnly, bool focus, Tree* tree);
	static int getNsites();
	static int getNsitesDisplayed();
	static int getNtaxa();
	static int getNtaxaDisplayed();
	static Alignment* getAlignment();
	static int getNumUniqueSequences();
	static Filtering* getFiltering();
	static void setOrderingToDirty();
	static void setSelectionToDirty();
	static bool colouringIsApplicable(Colouring* colouring);
	static Taxon* getTaxon(string label);
	static int getTaxonRowNum(Taxon* taxon);



	// Variables
	static Filtering* filtering;
	static bool selectionIsDirty;
	static bool orderingIsDirty;
	static int mostRecentlySelectedTaxon;
	static Alignment* THE_ALIGNMENT;

private:

};

#endif /* ALN_ALIGNMENTAPI_H_ */
