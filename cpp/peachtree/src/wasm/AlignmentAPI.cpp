
#include <iostream>
#include <emscripten.h>
#include <chrono>


#include "AlignmentAPI.h"
#include "PhylogenyAPI.h"
#include "EpiAPI.h"
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
 * Find the row number of this taxon in the filtering
 */
int AlignmentAPI::getTaxonRowNum(Taxon* taxon){
	return THE_ALIGNMENT->getTaxonIndex(taxon);
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



/*
 * Get the taxon object
 */
Taxon* AlignmentAPI::getTaxon(string label){
	return THE_ALIGNMENT->getTaxon(label);
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
jsonObject AlignmentAPI::getAlignmentGraphics(Scaling* scaling, double minNtWidth, double textSize, Colouring* colouring, int siteNumberingEvery) {
	return THE_ALIGNMENT->getAlignmentGraphics(scaling, colouring, minNtWidth, textSize, filtering, siteNumberingEvery);
}


int AlignmentAPI::getNumUniqueSequences(){
	if (AlignmentAPI::filtering == nullptr) return 0;
	return AlignmentAPI::filtering->getNumUniqueSequences();
}


void AlignmentAPI::resetHighlighting() {
	AlignmentAPI::THE_ALIGNMENT->clearHighlighting();
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

bool AlignmentAPI::isMock(){
	if (AlignmentAPI::THE_ALIGNMENT == nullptr) return false;
	return AlignmentAPI::THE_ALIGNMENT->getLength() == 0;
}

void AlignmentAPI::setSelectionToDirty(){
	AlignmentAPI::selectionIsDirty = true;
}


bool AlignmentAPI::colouringIsApplicable(Colouring* colouring){
	if (AlignmentAPI::THE_ALIGNMENT == nullptr) return true;
	if (colouring == nullptr) return true;
	return AlignmentAPI::THE_ALIGNMENT->getIsNucleotide() == colouring->isNucleotide();
}


void AlignmentAPI::cleanup(){
	if (AlignmentAPI::THE_ALIGNMENT != nullptr) {
		AlignmentAPI::THE_ALIGNMENT->cleanup();
		delete AlignmentAPI::THE_ALIGNMENT;
		AlignmentAPI::THE_ALIGNMENT = nullptr;
	}
	if (AlignmentAPI::filtering != nullptr){
		AlignmentAPI::filtering->cleanup();
		delete AlignmentAPI::filtering;
		AlignmentAPI::filtering = nullptr;
	}
	AlignmentAPI::selectionIsDirty = false;
	AlignmentAPI::mostRecentlySelectedTaxon = -1;
}


/*
 * Make a mock alignment so that taxa are labelled
 */
void AlignmentAPI::makeMockAlignment(Tree* tree){

	AlignmentAPI::cleanup();
	AlignmentAPI::THE_ALIGNMENT = new Alignment(tree);

	//AlignmentAPI::initFiltering(OptionsAPI::getVariantSitesOnly(), tree, nullptr);
	OptionsAPI::prepareColourings();

	EpiAPI::setEpiAccessionsToDirty();
	EpiAPI::validateAccessions(AlignmentAPI::THE_ALIGNMENT);
	OptionsAPI::resetWindowSize();

}




// Interface between javascript and cpp for webassembly
extern "C" {



	/*
	 * Remove the alignment
	 */
	void EMSCRIPTEN_KEEPALIVE removeAlignmentUpload(){

		AlignmentAPI::cleanup();

		// Tree
		PhylogenyAPI::orderingIsDirty = true;
		PhylogenyAPI::prepareLabelling(AlignmentAPI::getAlignment());
		PhylogenyAPI::orderingIsDirty = true;

		// Epi
		EpiAPI::setEpiAccessionsToDirty();
		EpiAPI::validateAccessions(AlignmentAPI::THE_ALIGNMENT);
		EpiAPI::setEpiAccessionsToDirty();

		OptionsAPI::resetScroll();
		OptionsAPI::resetWindowSize();

		WasmAPI::messageFromWasmToJS("");

	}


	void EMSCRIPTEN_KEEPALIVE uploadAlignment(){


		auto start = high_resolution_clock::now();
		char* str = WasmAPI::getFromHeap();

		AlignmentAPI::cleanup();


		cout << "uploading alignment " << strlen(str) << endl;


		string fasta(str);
		AlignmentAPI::THE_ALIGNMENT = new Alignment(fasta);


		AlignmentAPI::initFiltering(OptionsAPI::getVariantSitesOnly(), false, nullptr);
		OptionsAPI::prepareColourings();


		// If tree has been uploaded, check the alignment matches the tree
		PhylogenyAPI::setOrderingToDirty();
		PhylogenyAPI::prepareLabelling(AlignmentAPI::THE_ALIGNMENT);
		EpiAPI::setEpiAccessionsToDirty();
		EpiAPI::validateAccessions(AlignmentAPI::THE_ALIGNMENT);
		OptionsAPI::resetScroll();
		OptionsAPI::resetWindowSize();


		auto finish = high_resolution_clock::now();
		auto duration = duration_cast<seconds>(finish - start);


		if (Error::wasError()) {
			AlignmentAPI::cleanup();
			return;
		}

		cout << "Parsed successfully (" << duration.count() << "s)" << endl;

		json j;
		j["time"] = duration.count();
		WasmAPI::messageFromWasmToJS(j.dump(0));




	}



	/*
	 * Return a list of taxa labels, as a json string
	 */
	void EMSCRIPTEN_KEEPALIVE getListOfTaxaLabels(){

		jsonObject json;
		jsonObject labels = json::array();
		if (AlignmentAPI::THE_ALIGNMENT != nullptr) {

			// Find all displayed taxa
			for (Sequence* seq : AlignmentAPI::THE_ALIGNMENT->getSequences()) {
				Taxon* taxon = seq->getTaxon();
				if (AlignmentAPI::filtering->includeTaxon(taxon)) {
					labels.push_back(taxon->getName());
				}
			}

		}

		json["labels"] = labels;
		WasmAPI::messageFromWasmToJS(json.dump(0));

	}


	/**
	 * Select the taxon
	 * @param taxonNum
	 */
	void EMSCRIPTEN_KEEPALIVE selectTaxon(int taxonNum) {
		AlignmentAPI::mostRecentlySelectedTaxon = taxonNum;
		AlignmentAPI::THE_ALIGNMENT->selectTaxon(taxonNum);
		AlignmentAPI::setSelectionToDirty();
		WasmAPI::messageFromWasmToJS("{}");
	}

	/**
	 * Select/deselect everything between the most recently selected taxon and this one
	 */
	void EMSCRIPTEN_KEEPALIVE selectUpToTaxon(int newTaxonNum) {


		jsonObject toColour = json::array();


		// Toggle the currently selected one
		if (newTaxonNum == AlignmentAPI::mostRecentlySelectedTaxon) {
			bool isSelected = AlignmentAPI::THE_ALIGNMENT->selectTaxon(newTaxonNum);
			jsonObject obj;
			obj["i"] = newTaxonNum;
			obj["select"] = isSelected;
			toColour.push_back(obj);
		}
		else {

			// Set all the selected ones to the same state as the previously selected one
			bool setTo = AlignmentAPI::THE_ALIGNMENT->taxonIsSelected(AlignmentAPI::mostRecentlySelectedTaxon);

			if (newTaxonNum > AlignmentAPI::mostRecentlySelectedTaxon) {
				for (int i = AlignmentAPI::mostRecentlySelectedTaxon+1; i <= newTaxonNum; i ++ ) {

					Taxon* taxon = AlignmentAPI::THE_ALIGNMENT->getTaxon(i);
					if (AlignmentAPI::filtering != nullptr && !AlignmentAPI::filtering->includeTaxon(taxon)) continue;

					AlignmentAPI::THE_ALIGNMENT->selectTaxon(i, setTo);
					jsonObject obj;
					obj["i"] = i;
					obj["select"] = setTo;
					toColour.push_back(obj);
				}
			}else {
				for (int i = AlignmentAPI::mostRecentlySelectedTaxon-1; i >= newTaxonNum; i -- ) {

					Taxon* taxon = AlignmentAPI::THE_ALIGNMENT->getTaxon(i);
					if (AlignmentAPI::filtering != nullptr && !AlignmentAPI::filtering->includeTaxon(taxon)) continue;

					AlignmentAPI::THE_ALIGNMENT->selectTaxon(i, setTo);
					jsonObject obj;
					obj["i"] = i;
					obj["select"] = setTo;
					toColour.push_back(obj);
				}
			}
		}

		AlignmentAPI::setSelectionToDirty();
		WasmAPI::messageFromWasmToJS(toColour.dump(0));

	}

	/**
	 * Deselect all taxa
	 */
	void EMSCRIPTEN_KEEPALIVE clearSelection() {
		AlignmentAPI::THE_ALIGNMENT->clearSelection();
		AlignmentAPI::mostRecentlySelectedTaxon=-1;
		OptionsAPI::setFocusingOnTaxa(false);
		OptionsAPI::setFocusOnClade(false);
		OptionsAPI::resetWindowSize();
		AlignmentAPI::setSelectionToDirty();
		WasmAPI::messageFromWasmToJS("{}");
	}







}










