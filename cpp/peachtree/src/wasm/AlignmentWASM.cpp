
#include <iostream>
#include <emscripten.h>
#include <chrono>


#include "../api/AlignmentAPI.h"
#include "../api/PhylogenyAPI.h"
#include "../api/EpiAPI.h"
#include "../api/OptionsAPI.h"
#include "../error/Error.h"
#include "../epi/Epidemiology.h"

#include "WasmAPI.h"

using namespace std;
using namespace std::chrono;
using json = nlohmann::json;



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


		AlignmentAPI::initFiltering(OptionsAPI::getVariantSitesOnly(), false, false, nullptr);
		OptionsAPI::prepareColourings();


		// If tree has been uploaded, check the alignment matches the tree
		PhylogenyAPI::setOrderingToDirty();
		PhylogenyAPI::prepareLabelling(AlignmentAPI::THE_ALIGNMENT);
		EpiAPI::setEpiAccessionsToDirty();
		EpiAPI::validateAccessions(AlignmentAPI::THE_ALIGNMENT);
		for (Tree* tree : PhylogenyAPI::allTrees){
			AlignmentAPI::annotateTaxa(tree);
		}
		
		AlignmentAPI::annotateTaxa(EpiAPI::EPIDEMIOLOGY);
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
		//cout << "selecting " << taxonNum << endl;
		
		AlignmentAPI::mostRecentlySelectedTaxon = taxonNum;
		AlignmentAPI::THE_ALIGNMENT->selectTaxon(taxonNum);
		WasmAPI::messageFromWasmToJS("{}");
	}

	/**
	 * Select/deselect everything between the most recently selected taxon and this one
	 */
	void EMSCRIPTEN_KEEPALIVE selectUpToTaxon(int newTaxonNum) {


		jsonObject toColour = json::array();

		if (AlignmentAPI::mostRecentlySelectedTaxon == -1) {
			WasmAPI::messageFromWasmToJS(toColour.dump(0));
		}

		


		// Toggle the currently selected one
		if (newTaxonNum == AlignmentAPI::mostRecentlySelectedTaxon) {
			bool isSelected = AlignmentAPI::THE_ALIGNMENT->selectTaxon(newTaxonNum);
			jsonObject obj;
			obj["i"] = newTaxonNum;
			obj["select"] = isSelected;
			toColour.push_back(obj);
		}
		else {
			
			
			
			int newTaxonIndex = AlignmentAPI::THE_ALIGNMENT->getTaxonIndex(newTaxonNum);
			int prevTaxonIndex = AlignmentAPI::THE_ALIGNMENT->getTaxonIndex(AlignmentAPI::mostRecentlySelectedTaxon);
			
			
			// Are they all in the same state?
			bool setTo = true;
			bool thereExistsSelected = false;
			bool thereExistsDeselected = false;
			if (newTaxonIndex > prevTaxonIndex) {
				for (int i = prevTaxonIndex; i <= newTaxonIndex; i ++ ) {

					Taxon* taxon = AlignmentAPI::THE_ALIGNMENT->getTaxonFromIndex(i);
					if (AlignmentAPI::filtering != nullptr && !AlignmentAPI::filtering->includeTaxon(taxon)) continue;
					if (taxon->getIsSelected()) thereExistsSelected = true;
					else thereExistsDeselected = true;
					
					if (thereExistsSelected && thereExistsDeselected) break;
				}
			}else {
				for (int i = prevTaxonIndex; i >= newTaxonIndex; i -- ) {

					Taxon* taxon = AlignmentAPI::THE_ALIGNMENT->getTaxonFromIndex(i);
					if (AlignmentAPI::filtering != nullptr && !AlignmentAPI::filtering->includeTaxon(taxon)) continue;
					if (taxon->getIsSelected()) thereExistsSelected = true;
					else thereExistsDeselected = true;
					
					if (thereExistsSelected && thereExistsDeselected) break;
					
				}
				
				
				
			}
			
			// If all are the same state, then toggle all
			if (thereExistsSelected != thereExistsDeselected) {
				if (thereExistsSelected) setTo = false;
				else setTo = true;
			}
			
			// Else set all the selected ones to the same state as the previously selected one
			else{
				setTo = AlignmentAPI::THE_ALIGNMENT->taxonIsSelected(AlignmentAPI::mostRecentlySelectedTaxon);
			}

			
			

			if (newTaxonIndex > prevTaxonIndex) {
				for (int i = prevTaxonIndex; i <= newTaxonIndex; i ++ ) {

					Taxon* taxon = AlignmentAPI::THE_ALIGNMENT->getTaxonFromIndex(i);
					if (AlignmentAPI::filtering != nullptr && !AlignmentAPI::filtering->includeTaxon(taxon)) continue;

					AlignmentAPI::THE_ALIGNMENT->selectTaxon(i, setTo);
					jsonObject obj;
					obj["i"] = i;
					obj["select"] = setTo;
					toColour.push_back(obj);
				}
			}else {
				for (int i = prevTaxonIndex; i >= newTaxonIndex; i -- ) {

					Taxon* taxon = AlignmentAPI::THE_ALIGNMENT->getTaxonFromIndex(i);
					if (AlignmentAPI::filtering != nullptr && !AlignmentAPI::filtering->includeTaxon(taxon)) continue;

					AlignmentAPI::THE_ALIGNMENT->selectTaxon(i, setTo);
					jsonObject obj;
					obj["i"] = i;
					obj["select"] = setTo;
					toColour.push_back(obj);
				}
			}
			
			if (setTo == false) AlignmentAPI::mostRecentlySelectedTaxon = -1;
			
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




	/**
	 *Download the alignment
	 */
	void EMSCRIPTEN_KEEPALIVE downloadAlignment() {
		
		jsonObject contents;
		if (AlignmentAPI::THE_ALIGNMENT != nullptr && !AlignmentAPI::isMock()){
			contents["contents"] = AlignmentAPI::THE_ALIGNMENT->toFasta(AlignmentAPI::filtering, OptionsAPI::sampleNameAnnotation->getVal());
		}
		WasmAPI::messageFromWasmToJS(contents.dump(0));
		
	}



}










