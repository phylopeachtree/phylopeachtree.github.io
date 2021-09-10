/*
 * OptionsAPI.cpp
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include <emscripten.h>
#include "../api/OptionsAPI.h"
#include "../api/AlignmentAPI.h"
#include "../api/PhylogenyAPI.h"
#include "../api/EpiAPI.h"
#include "WasmAPI.h"
#include "../error/Error.h"
#include "../aln/colourings/Colouring.h"
#include "../aln/colourings/AliView.h"
#include "../aln/colourings/JalView.h"
#include "../aln/colourings/Drums.h"
#include "../aln/colourings/ClustalAmino.h"
#include "../phy/ClusterTree.h"
#include "../epi/Timeline.h"


// Interface between javascript and cpp for webassembly
extern "C" {


	void EMSCRIPTEN_KEEPALIVE init(){


		OptionsAPI::init();
		WasmAPI::messageFromWasmToJS("done");

	}



	/**
	 * Build a tree from the alignment
	 */
	void EMSCRIPTEN_KEEPALIVE buildTree() {

		LinkType method = ClusterTree::getLinkType(OptionsAPI::treeMethods->getVal());
		jsonObject j = PhylogenyAPI::buildTree(AlignmentAPI::getAlignment(), method);
		if (Error::wasError()) {
			return;
		}
		WasmAPI::messageFromWasmToJS(j.dump(0));

	}
	
	
	
	/**
	 * Reorder children in the transmission tree to improve compatibility with infectious periods
	 */
	void EMSCRIPTEN_KEEPALIVE reorderTree() {
		OptionsAPI::reorderTree();
		WasmAPI::messageFromWasmToJS("");
	}
	
	


	/**
	 * Generate all objects. Now ready - to render onto the svg
	 * Return canvas width and height but do not return any of the objects until getGraphics is called
	*/
	void EMSCRIPTEN_KEEPALIVE initGraphics(double maxH, double maxW, int downloadInt) {
		
		// Return
		jsonObject json = OptionsAPI::initGraphics(maxH, maxW, downloadInt);
		WasmAPI::messageFromWasmToJS(json.dump(0));

	}



	/*
	 * Return the json objects generated by initGraphics 1 chunk at a time
	*/
	void EMSCRIPTEN_KEEPALIVE getGraphics() {


		//cout << "graphics size " << OptionsAPI::graphicalObjects.size() << endl;

		jsonObject small = json::array();

		if (OptionsAPI::graphicalObjects == nullptr || OptionsAPI::graphicalObjects.size() == 0) {
			WasmAPI::messageFromWasmToJS(small.dump(0));
			return;
		}


		// Keep adding to the string until the string is too large
		int len = 0;
		bool addedObj = false;
		do {
			jsonObject thisJson = OptionsAPI::graphicalObjects.at(0);
			int thisLen = thisJson.dump(0).size();
			if (thisLen + len < OptionsAPI::CHUNK_SIZE) {

				//System.out.println("Added json at position " + i + ". Total length " + );

				// Update string
				addedObj = true;
				small.push_back(thisJson);
				len += thisLen;

				// Remove 1st element from from array
				OptionsAPI::graphicalObjects.erase(0);

			}else {

				// Chunk is too big. Move on.
				//cout << "big chunk" << endl;
				break;
			}

		}while(OptionsAPI::graphicalObjects.size() > 0);


		if (!addedObj) {
			cout << "Error: chunk sizes are too small" << endl;
		}



		WasmAPI::messageFromWasmToJS(small.dump(0));

	}







	/**
	 * Scroll up/down slightly
	*/
	void EMSCRIPTEN_KEEPALIVE scrollABitVertical(int dir) {
		double y1 = OptionsAPI::scrollY->getVal();
		double relativeScrollAmnt = OptionsAPI::SCROLL_Y_NROWS/AlignmentAPI::getNtaxaDisplayed();
		double y2 = y1 + dir*relativeScrollAmnt;
		OptionsAPI::scrollY->setVal(y2);
		//cout << "scrolling " << y1 << " -> " << y2 << " / " << OptionsAPI::scrollY->getVal() << endl;
		
		jsonObject result;
		if (dir < 0 && y1 <= OptionsAPI::scrollY->getMin()) result["changed"] = false;
		else if (dir > 0 && y1 >= OptionsAPI::scrollY->getMax()) result["changed"] = false;
		else result["changed"] = true;

		WasmAPI::messageFromWasmToJS(result.dump(0));
	}
	
	
	/**
	 * Scroll left/right slightly
	*/
	void EMSCRIPTEN_KEEPALIVE scrollABitHorizontal(int dir) {
		double x1 = OptionsAPI::scrollX->getVal();
		double relativeScrollAmnt = OptionsAPI::SCROLL_Y_NROWS/AlignmentAPI::getNsitesDisplayed();
		double x2 = x1 + dir*relativeScrollAmnt;
		OptionsAPI::scrollX->setVal(x2);

		
		
		jsonObject result;
		if (dir < 0 && x1 <= OptionsAPI::scrollX->getMin()) result["changed"] = false;
		else if (dir > 0 && x1 >= OptionsAPI::scrollX->getMax()) result["changed"] = false;
		else result["changed"] = true;
		
		WasmAPI::messageFromWasmToJS(result.dump(0));
	}
	
	
	
		/**
	 * Zoom in/out slightly
	*/
	void EMSCRIPTEN_KEEPALIVE zoomABit(int dir) {
		double y1 = OptionsAPI::zoomScale->getVal();
		double y2 = y1 + dir*OptionsAPI::zoomScale->getStepSize();
		OptionsAPI::zoomScale->setVal(y2);
	
		
		
		jsonObject result;
		if (dir < 0 && y1 <= OptionsAPI::zoomScale->getMin()) result["changed"] = false;
		else if (dir > 0 && y1 >= OptionsAPI::zoomScale->getMax()) result["changed"] = false;
		else result["changed"] = true;
		
		WasmAPI::messageFromWasmToJS(result.dump(0));
	}




	/*
	 * Set the value of this option
	*/
	void EMSCRIPTEN_KEEPALIVE setOption(char* charID, char* charVal) {


		string id(charID);


		// Find the option
		Option* option = nullptr;
		for (Option* o : OptionsAPI::getOptionList()) {
			if (o->getName() == id) {
				option = o;
				break;
			}
		}


		string value(charVal);

		if (option == nullptr) {
			cout << "Cannot find option " << id << endl;
		}

		else {

			// instanceof
			if (NumericalOption* v = dynamic_cast<NumericalOption*>(option)) {

				double val = stod(value);



				// Special case: scroll bar positions should be normalised
				if (option == OptionsAPI::scrollX) {
					//val = val - (OptionsAPI::canvasWidth->getVal())*OptionsAPI::division2->getVal();
					//val = val / (OptionsAPI::canvasWidth->getVal() - (OptionsAPI::canvasWidth->getVal())*OptionsAPI::division2->getVal());
				}

				if (option == OptionsAPI::scrollY) {
					cout << "scrolly " << val << endl;
					//val = val / OptionsAPI::canvasHeight->getVal(); // - OptionsAPI::TOP_MARGIN-OptionsAPI::MARGIN_SIZE);
				}



				v->setVal(val);
			}

			else if (BooleanOption* v = dynamic_cast<BooleanOption*>(option)) {

				bool val = value == "true" || value == "1";

				// Special case: if focusOnClade is enabled, then enable focusOnTaxa too
				if (option == OptionsAPI::focusOnClade && val == true) {
					OptionsAPI::setFocusingOnTaxa(true);
				}

				// If focusOnTaxa is enabled, then set focusOnClade to false
				if (option == OptionsAPI::focusOnTaxa && val == true) {
					OptionsAPI::setFocusOnClade(false);
				}

				if (option == OptionsAPI::focusOnClade || option == OptionsAPI::focusOnTaxa) {
					OptionsAPI::resetScroll();
					AlignmentAPI::setSelectionToDirty();
					AlignmentAPI::resetHighlighting();
				}


				// If 'displayIncompatibleTranmissions' becomes true, set 'transmissionTree' to true
				if (option == OptionsAPI::displayIncompatibleTranmissions && val == true){
					OptionsAPI::transmissionTree->setVal(true);
					reorderTree();
				}
				


				//cout << "setting " << option->getName() << " to " << val << "|" << value << endl;

				v->setVal(val);
			}

			else if (DiscreteOption* v = dynamic_cast<DiscreteOption*>(option)) {
				v->setVal(value);
			}

			else if (ColourOption* v = dynamic_cast<ColourOption*>(option)) {
				v->setVal(value);
			}


		}


		WasmAPI::messageFromWasmToJS("");

	}


	/**
	 * Build a tree from the alignment
	 * @return

	string EMSCRIPTEN_KEEPALIVE buildTree () {

		try {

			LinkType method = LinkType.valueOf(((DiscreteOption)treeMethods).getVal().tostring());
			System.out.println("building tree from " + method.tostring());

			return PhylogenyAPI.buildTree(AlignmentAPI.getAlignment(), method);
		} catch (Exception e) {
			return getErrorJSON(e);
		}

	}
 */





	/**
	 * Declare the current taxon label as the focal label so it can be zoomed in on and selected
	 */
	void EMSCRIPTEN_KEEPALIVE searchForTaxon(char* c) {


		string label(c);

		OptionsAPI::focalTaxon = nullptr;


		// Get the taxon
		Taxon* taxon = AlignmentAPI::getTaxon(label);
		if (taxon == nullptr) {
			cout << "Warning: cannot find " << label << endl;
			return;
		}

		//System.out.println("Found " + taxon.getName());


		// Select it
		taxon->setIsSelected(true);
		taxon->setIsHighlighted(true);


		// Set it as the focal taxon
		OptionsAPI::focalTaxon = taxon;


		WasmAPI::messageFromWasmToJS("");



	}




	/**
	 * Is the system ready to render?
	 */
	void EMSCRIPTEN_KEEPALIVE isReady() {
		jsonObject json;
		json["ready"] = OptionsAPI::isReady();
		WasmAPI::messageFromWasmToJS(json.dump(0));
	}
	
	
	/**
	* Is the tree ready?
	*/
	void EMSCRIPTEN_KEEPALIVE treeIsReady() {
		jsonObject json;
		json["ready"] = PhylogenyAPI::isReady();
		WasmAPI::messageFromWasmToJS(json.dump(0));
	}
	
	
	/**
	* Is the alignment ready?
	*/
	void EMSCRIPTEN_KEEPALIVE alignmentIsMock() {
		jsonObject json;
		json["mock"] = AlignmentAPI::isMock();
		WasmAPI::messageFromWasmToJS(json.dump(0));
	}
	
	
	
	/**
	* Can the tree be reordered as a transmission tree
	*/
	void EMSCRIPTEN_KEEPALIVE transmissionTreeCanBeReordered() {
		jsonObject json;
		json["ready"] = EpiAPI::getTimeline() != nullptr && PhylogenyAPI::THE_TREE != nullptr && 
						OptionsAPI::epiSymptomDate->getVal() != "None" && OptionsAPI::transmissionTree->getVal()
						&& EpiAPI::getTimeline()->isReady();
		WasmAPI::messageFromWasmToJS(json.dump(0));
	}
	


	/**
	 * Get options list
	 * @return
	 */
	void EMSCRIPTEN_KEEPALIVE getOptions() {

		jsonObject arr;
		for (Option* option : OptionsAPI::getOptionList()) {
			if (option == nullptr) continue;
			arr.push_back(option->toJSON());
		}
		WasmAPI::messageFromWasmToJS(arr.dump(0));

	}
	
	
	/**
	 * Download the tree
	 */
	void EMSCRIPTEN_KEEPALIVE downloadSamples() {
		
		jsonObject contents;
		if (AlignmentAPI::THE_ALIGNMENT != nullptr){
			bool reportInfections = OptionsAPI::reportNumberOfInfections->getVal() && PhylogenyAPI::isReady();
			contents["contents"] = AlignmentAPI::downloadSamples(OptionsAPI::sampleNameAnnotation->getVal(), OptionsAPI::displayMissingPercentage->getVal(), reportInfections);
		}
		WasmAPI::messageFromWasmToJS(contents.dump(0));
		
	}
	




}












