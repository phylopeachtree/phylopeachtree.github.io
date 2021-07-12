
#include <iostream>
#include <emscripten.h>
#include <chrono>


#include "AlignmentAPI.h"
#include "PhylogenyAPI.h"
#include "OptionsAPI.h"
#include "../error/Error.h"

#include "WasmAPI.h"

using namespace std;
using namespace std::chrono;
using json = nlohmann::json;


Alignment* AlignmentAPI::THE_ALIGNMENT = nullptr;
Filtering* AlignmentAPI::filtering = nullptr;
bool AlignmentAPI::selectionIsDirty = false;
bool AlignmentAPI::orderingIsDirty = false;
int AlignmentAPI::mostRecentlySelectedTaxon = -1;


/*
 * Is the alignment ready to render?
 */
bool AlignmentAPI::isReady(){
	return AlignmentAPI::THE_ALIGNMENT != nullptr;
}


/*
 * Initialise filtering object
 */
void AlignmentAPI::initFiltering(bool variantSitesOnly, bool focus, Tree* tree){


	if (AlignmentAPI::THE_ALIGNMENT == nullptr) return;

	bool initRequired = false;
	if (filtering == nullptr) initRequired = true;
	else if (filtering->getFocusing() && selectionIsDirty) initRequired = true;
	else if (filtering->variantSitesOnlyParsed() != variantSitesOnly) initRequired = true;
	else if (filtering->getFocusing() != focus) initRequired = true;
	else if (filtering->getTree() != tree) initRequired = true;
	else if (orderingIsDirty) initRequired = true;


	// Default filtering
	if (initRequired) {
		filtering = new Filtering(variantSitesOnly, focus, THE_ALIGNMENT, tree);
		selectionIsDirty = false;
		orderingIsDirty = false;
	}


}


/**
 * Get taxon graphics json
 */
jsonObject AlignmentAPI::getTaxaGraphics(Scaling* scaling, double textSize, bool showTaxonNumbers){
	return THE_ALIGNMENT->getTaxaGraphics(scaling, textSize, filtering, showTaxonNumbers);
}


/*
 * Get alignment graphics
 */
jsonObject AlignmentAPI::getAlignmentGraphics(Scaling* scaling, double minNtWidth, double textSize, Colouring* colouring) {
	return THE_ALIGNMENT->getAlignmentGraphics(scaling, colouring, minNtWidth, textSize, filtering);
}


int AlignmentAPI::getNumUniqueSequences(){
	if (AlignmentAPI::filtering == nullptr) return 0;
	return AlignmentAPI::filtering->getNumUniqueSequences();
}


int AlignmentAPI::getNsites(){
	if (AlignmentAPI::THE_ALIGNMENT == nullptr) return 0;
	return AlignmentAPI::THE_ALIGNMENT->getLength();
}

int AlignmentAPI::getNsitesDisplayed(){
	if (AlignmentAPI::THE_ALIGNMENT == nullptr) return 0;
	if (AlignmentAPI::filtering == nullptr) return 0;
	return AlignmentAPI::filtering->getNumSites();
}

int AlignmentAPI::getNtaxa(){
	if (AlignmentAPI::THE_ALIGNMENT == nullptr) return 0;
	return THE_ALIGNMENT->getNtaxa();
}

int AlignmentAPI::getNtaxaDisplayed(){
	if (AlignmentAPI::THE_ALIGNMENT == nullptr) return 0;
	if (AlignmentAPI::filtering == nullptr) return 0;
	return AlignmentAPI::filtering->getNumSeqs();

}

Alignment* AlignmentAPI::getAlignment(){
	return AlignmentAPI::THE_ALIGNMENT;
}

Filtering* AlignmentAPI::getFiltering(){
	return AlignmentAPI::filtering;
}

void AlignmentAPI::setOrderingToDirty(){
	AlignmentAPI::orderingIsDirty = true;
}

void AlignmentAPI::setSelectionToDirty(){
	AlignmentAPI::selectionIsDirty = true;
}


bool AlignmentAPI::colouringIsApplicable(Colouring* colouring){
	if (AlignmentAPI::THE_ALIGNMENT == nullptr) return true;
	if (colouring == nullptr) return true;
	return AlignmentAPI::THE_ALIGNMENT->getIsNucleotide() == colouring->isNucleotide();
}

// Interface between javascript and cpp for webassembly
extern "C" {

	void EMSCRIPTEN_KEEPALIVE uploadAlignment(){


		auto start = high_resolution_clock::now();

		char* str = WasmAPI::getFromHeap();
		AlignmentAPI::filtering = nullptr;
		AlignmentAPI::selectionIsDirty = false;
		AlignmentAPI::mostRecentlySelectedTaxon = -1;

		cout << "uploading alignment " << strlen(str) << endl;


		string fasta(str);
		AlignmentAPI::THE_ALIGNMENT = new Alignment(fasta);
		AlignmentAPI::initFiltering(OptionsAPI::getVariantSitesOnly(), false, nullptr);
		OptionsAPI::prepareColourings();


		// If tree has been uploaded, check the alignment matches the tree
		PhylogenyAPI::prepareLabelling(AlignmentAPI::THE_ALIGNMENT);
		// TODO EpiAPI.setEpiAccessionsToDirty();
		// TODO EpiAPI.validateAccessions(THE_ALIGNMENT);
		OptionsAPI::resetScroll();
		OptionsAPI::resetWindowSize();


		auto finish = high_resolution_clock::now();
		auto duration = duration_cast<seconds>(finish - start);




		if (Error::wasError()) {
			AlignmentAPI::THE_ALIGNMENT = nullptr;
			return;
		}

		cout << "Parsed successfully (" << duration.count() << "s)" << endl;

		json j;
		j["time"] = duration.count();
		WasmAPI::messageFromWasmToJS(j.dump(0));


	}


}

