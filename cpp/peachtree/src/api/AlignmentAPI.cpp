
#include <iostream>
#include <chrono>


#include "AlignmentAPI.h"
#include "PhylogenyAPI.h"
#include "EpiAPI.h"
#include "OptionsAPI.h"
#include "../error/Error.h"
#include "../epi/Epidemiology.h"



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
 * Set alignment
 */
void AlignmentAPI::setAlignment(Alignment* aln){
	THE_ALIGNMENT = aln;
	selectionIsDirty = true;
	orderingIsDirty = true;
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
void AlignmentAPI::initFiltering(bool variantSitesOnly, bool focus, bool focusOnClade, Tree* tree){

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
		
		
		//cout << "initRequired: " << selectionIsDirty << "," << filtering->getFocusing() << "," << orderingIsDirty << "," << (filtering->getTree() != tree) << endl;
		
		// Cleanup
		if (filtering == nullptr) {
			filtering = new Filtering(variantSitesOnly, focus, focusOnClade, THE_ALIGNMENT, tree);
			
		}else{
			filtering->init(variantSitesOnly, focus, focusOnClade, THE_ALIGNMENT, tree);
		}

		
		if (focus || tree != nullptr){
			AlignmentAPI::THE_ALIGNMENT->clearSelection();
			AlignmentAPI::THE_ALIGNMENT->clearHighlighting();
		}
	}
	
	selectionIsDirty = false;
	orderingIsDirty = false;

}


/*
 * Are any samples highlighted? 
 */
bool AlignmentAPI::getHighlighted(){
	
	for (Sequence* seq : AlignmentAPI::THE_ALIGNMENT->getSequences()){
		if (seq->getTaxon()->getIsHighlighted()) return true;
	}
	
	return false;
}

/*
	Annotate taxa with epidemological information
*/
void AlignmentAPI::annotateTaxa(Epidemiology* epi){
	if (THE_ALIGNMENT == nullptr) return;
	if (epi == nullptr) return;
	THE_ALIGNMENT->annotateTaxa(epi);
}

/*
	Annotate taxa with tree information
*/
void AlignmentAPI::annotateTaxa(Tree* tree){
	if (THE_ALIGNMENT == nullptr) return;
	if (tree == nullptr) return;
	THE_ALIGNMENT->annotateTaxa(tree);
}


/*
	Download the samples and their displayed metadata
*/
string AlignmentAPI::downloadSamples(string sampleNameAnnotation, bool displayMissingPercentage, bool reportInfections){
	if (THE_ALIGNMENT == nullptr) return "";
	return THE_ALIGNMENT->downloadSamples(filtering, sampleNameAnnotation, displayMissingPercentage, reportInfections);
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
jsonObject AlignmentAPI::getTaxaGraphics(Scaling* scaling, double textSize, bool showTaxonNumbers, bool displayMissingPercentage, string sampleNameAnnotation, bool reportInfections){
	return THE_ALIGNMENT->getTaxaGraphics(scaling, textSize, filtering, showTaxonNumbers, displayMissingPercentage, sampleNameAnnotation, reportInfections);
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
	//cout << "setSelectionToDirty" << endl;
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
	OptionsAPI::resetWindowSize(true, false);
	
	AlignmentAPI::annotateTaxa(tree);
	AlignmentAPI::annotateTaxa(EpiAPI::EPIDEMIOLOGY);

}


