/*
 * OptionsAPI.cpp
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */

#include <emscripten.h>
#include "OptionsAPI.h"
#include "AlignmentAPI.h"
#include "PhylogenyAPI.h"
#include "EpiAPI.h"
#include "WasmAPI.h"
#include "../error/Error.h"
#include "../aln/colourings/Colouring.h"
#include "../aln/colourings/AliView.h"
#include "../aln/colourings/JalView.h"
#include "../aln/colourings/Drums.h"
#include "../aln/colourings/ClustalAmino.h"
#include "../phy/ClusterTree.h"



const long OptionsAPI::CHUNK_SIZE = 40000;
const double OptionsAPI::LEFT_MARGIN = 16; // Left hand margin
const double OptionsAPI::TOP_MARGIN = 16; // Top margin
const double OptionsAPI::INIT_WIDTH = 1200;
const double OptionsAPI::INIT_HEIGHT = INIT_WIDTH*0.618;
const double OptionsAPI::INIT_DIV1 = 0.3;
const double OptionsAPI::INIT_DIV2 = 0.5;
const double OptionsAPI::SCROLL_Y_NROWS = 10;



// Boundaries
NumericalOption* OptionsAPI::canvasWidth = new NumericalOption("width", "General", "Width of canvas", INIT_WIDTH, 10, 2000, 100, true);
NumericalOption* OptionsAPI::canvasHeight = new NumericalOption("height", "General", "Height of canvas", INIT_HEIGHT, 10, 2000, 100, true);
NumericalOption* OptionsAPI::division1 = new NumericalOption("division1", "General", "Relative position of the tree/taxa boundary", 0, 0, 1, 0.1, true);
NumericalOption* OptionsAPI::division2 = new NumericalOption("division2", "General", "Relative position of the taxa/alignment boundary", 0.2, 0, 1, 0.1, true);



// Scroll bars
NumericalOption* OptionsAPI::scrollY  = new NumericalOption("scrollY", "General", "Relative position of y-scrollbar", 0, 0, 1, 0.1, true);
NumericalOption* OptionsAPI::scrollX  = new NumericalOption("scrollX", "General", "Relative position of x-scrollbar", 0, 0, 1, 0.1, true);


// Phylogeny
DiscreteOption* OptionsAPI::treeMethods;
NumericalOption* OptionsAPI::branchwidth = new NumericalOption("branchWidth", "Phylogeny", "Branch width", 2, 0.25, 20, 0.5);
NumericalOption* OptionsAPI::nodeRadius = new NumericalOption("nodeRadius", "Phylogeny", "Node radius", 3, 0, 20, 0.5);
NumericalOption* OptionsAPI::treeSpacing = new NumericalOption("treeSpacing", "Phylogeny", "Horizontal padding around tree", 5, 0, 50, 5);
BooleanOption* OptionsAPI::showTaxaOnTree = new BooleanOption("showTaxaOnTree", "Phylogeny", "Indicate taxa on tree", true);
BooleanOption* OptionsAPI::transmissionTree = new BooleanOption("transmissionTree", "Phylogeny", "Display as a transmission tree", false);
DiscreteOption* OptionsAPI::internalNodeLabels;
DiscreteOption* OptionsAPI::leafNodeLabels;
NumericalOption* OptionsAPI::annotationFontSize = new NumericalOption("annotationFontSize", "Phylogeny", "Tree annotation font size", 8, 0, 14, 1, true);
NumericalOption* OptionsAPI::annotationRounding = new NumericalOption("annotationRounding", "Phylogeny", "Tree annotation sf", 3, 1, 8, 1, true);

// Samples
NumericalOption* OptionsAPI::siteHeight = new NumericalOption("siteHeight", "Samples", "Row heights", 20, 1, 100, 5);
NumericalOption* OptionsAPI::fontSizeTaxa = new NumericalOption("fontSizeTaxa", "Samples", "Font size of samples", 13, 0, 50, 1);
NumericalOption* OptionsAPI::taxaSpacing = new NumericalOption("taxaSpacing", "Samples", "Padding before sample names", 5, 0, 50, 1);
BooleanOption* OptionsAPI::showTaxonNumbers = new BooleanOption("showTaxonNumbers", "Samples", "Show sample numbers", false);
BooleanOption* OptionsAPI::focusOnTaxa = new BooleanOption("focusOnTaxa", "Samples", "Show only selected samples", false, true);
BooleanOption* OptionsAPI::focusOnClade = new BooleanOption("focusOnClade", "Samples", "Show only clade of selected samples", false, true);

// Alignment
NumericalOption* OptionsAPI::ntWidth = new NumericalOption("ntWidth", "Alignment", "Width of alignment sites", 15, 1, 100, 5);
NumericalOption* OptionsAPI::fontSizeAln = new NumericalOption("fontSizeAln", "Alignment", "Font size of alignment", 16, 0, 50, 1);
BooleanOption* OptionsAPI::variantSitesOnly = new BooleanOption("variantSitesOnly", "Alignment", "Show segregating sites only", true);
DiscreteOption* OptionsAPI::siteColourType;
DiscreteOption* OptionsAPI::colourings;


// Epidemiology
DiscreteOption* OptionsAPI::epiSymptomDate;
NumericalOption* OptionsAPI::infectiousPeriodBefore = new NumericalOption("infectiousPeriodBefore", "Epidemiology", "Number of days infectious before symptom onset", 3, 0, 28, 1);
NumericalOption* OptionsAPI::infectiousPeriodAfter = new NumericalOption("infectiousPeriodAfter", "Epidemiology", "Number of days infectious after symptom onset", 7, 0, 28, 1);



// Variables
jsonObject OptionsAPI::graphicalObjects = nullptr;
Taxon* OptionsAPI::focalTaxon = nullptr;
vector<Colouring*> OptionsAPI::colouringClasses;



/*
 * Get a list of options
*/
 vector<Option*> OptionsAPI::getOptionList(){

	 // No reflection
	vector<Option*> options;

	// Boundaries
	options.push_back(canvasWidth);
	options.push_back(canvasHeight);
	options.push_back(division1);
	options.push_back(division2);

	// Scroll bars
	options.push_back(scrollX);
	options.push_back(scrollY);

	// Phylogeny
	options.push_back(treeMethods);
	options.push_back(branchwidth);
	options.push_back(nodeRadius);
	options.push_back(treeSpacing);
	options.push_back(showTaxaOnTree);
	options.push_back(transmissionTree);
	options.push_back(internalNodeLabels);
	options.push_back(leafNodeLabels);
	options.push_back(annotationFontSize);
	options.push_back(annotationRounding);

	// Taxa
	options.push_back(siteHeight);
	options.push_back(fontSizeTaxa);
	options.push_back(taxaSpacing);
	options.push_back(showTaxonNumbers);
	options.push_back(focusOnTaxa);
	options.push_back(focusOnClade);

	// Alignment
	options.push_back(ntWidth);
	options.push_back(fontSizeAln);
	options.push_back(variantSitesOnly);
	options.push_back(siteColourType);
	options.push_back(colourings);

	// Epidemiology
	options.push_back(epiSymptomDate);
	options.push_back(infectiousPeriodBefore);
	options.push_back(infectiousPeriodAfter);


	return options;

}



/**
 * Prepare epidemiology annotations
*/
void OptionsAPI::prepareEpiAnnotations() {


	// Get annotations
	vector<string> annotations = EpiAPI::getAllAnnotations();
	vector<string> vals;
	vals.push_back("none");
	for (string a : annotations) vals.push_back(a);

	// Options
	OptionsAPI::epiSymptomDate = new DiscreteOption("epiSymptomDate", "Epidemiology", "Symptom onset date", vals.at(0), vals);

}




/*
 * Ready for graphics?
 */
bool OptionsAPI::isReady(){
	return AlignmentAPI::isReady();
}




/**
 * Set whether taxa are being focused on
 * @return
 */
void OptionsAPI::setFocusingOnTaxa(bool val) {
	OptionsAPI::focusOnTaxa->setVal(val);
}


/**
 * Set whether a clade is being focused on
 * @return
 */
void OptionsAPI::setFocusOnClade(bool val) {
	OptionsAPI::focusOnClade->setVal(val);
}



/**
 * Are selected taxa being focused on
 * @return
 */
bool OptionsAPI::getFocusingOnTaxa() {
	return OptionsAPI::focusOnTaxa->getVal();
}


/**
 * Is a clade being focused on
 * @return
 */
bool OptionsAPI::getFocusOnClade () {
	return OptionsAPI::focusOnTaxa->getVal();
}



/**
 * Are variant sites only being displayed
 */
bool OptionsAPI::getVariantSitesOnly() {
	return OptionsAPI::variantSitesOnly->getVal();
}


/**
 * Reset the scrollbars to 0,0
*/
void OptionsAPI::resetScroll() {
	OptionsAPI::scrollX->setVal(0);
	OptionsAPI::scrollY->setVal(0);
}



/*
 * Reset window width/height
*/
void OptionsAPI::resetWindowSize () {
	OptionsAPI::canvasWidth->setVal(OptionsAPI::INIT_WIDTH);
	OptionsAPI::canvasHeight->setVal(OptionsAPI::INIT_HEIGHT);
}





/*
 * Prepares the colouring option so that only colour schemes applicable to the current alignment (aa or nt) are included
 * @throws IllegalAccessException
 * @throws InstantiationException
*/
void OptionsAPI::prepareColourings() {


	vector<string> colourClassNames;
	for (Colouring* col : OptionsAPI::colouringClasses) {
        if (AlignmentAPI::colouringIsApplicable(col)) {
        	 cout << col->getName() << " is applicable" << endl;
        	 colourClassNames.push_back(col->getName());
        }

	}

	colourings = new DiscreteOption("colourings", "Alignment", "Alignment colour scheme", colourClassNames.at(0), colourClassNames);

}


/**
 * Get the selected colouring object
 */
Colouring* OptionsAPI::getSelectedColouring(){

	string val = OptionsAPI::colourings->getVal();
	for (Colouring* col : OptionsAPI::colouringClasses) {
		if (val == col->getName()) return col;
	}

	Error::throwError("Cannot locate colour scheme " + val);
	return nullptr;

}


/*
 * Prepares all node annotations
 */
void OptionsAPI::prepareTreeAnnotationOptions(){

	if (INIT_DIV1 > division2->getVal()) {
		division2->setVal(INIT_DIV2);
	}
	division1->setVal(INIT_DIV1);

	vector<string> annotations;
	annotations.push_back("None");
	vector<string> annotations2 = PhylogenyAPI::getAllAnnotations();
	annotations.insert(annotations.end(), annotations2.begin(), annotations2.end());
	vector<string> annotations3 = EpiAPI::getAllAnnotations();
	annotations.insert(annotations.end(), annotations3.begin(), annotations3.end());

	if (annotations.size() == 1) {
		annotationFontSize->hide();
		annotationRounding->hide();
		internalNodeLabels = nullptr;
		leafNodeLabels = nullptr;
		return;
	}

	annotationFontSize->show();
	annotationRounding->show();
	internalNodeLabels = new DiscreteOption("internalNodeLabels", "Phylogeny", "Internal node labels", annotations.at(0), annotations);
	leafNodeLabels = new DiscreteOption("leafNodeLabels", "Phylogeny", "Leaf labels", annotations.at(0), annotations);



}


// Interface between javascript and cpp for webassembly
extern "C" {


	void EMSCRIPTEN_KEEPALIVE init(){


		// TODO
		cout << "Initialising options..." << endl;
		OptionsAPI::graphicalObjects.clear();
		OptionsAPI::focalTaxon = nullptr;

		OptionsAPI::internalNodeLabels = nullptr;
		OptionsAPI::leafNodeLabels = nullptr;
		//epiSymptomDate = null;


		OptionsAPI::treeMethods = new DiscreteOption("treeMethods", "Phylogeny", "Method for phylogenetic tree estimation", ClusterTree::getDefaultLinkType(), ClusterTree::getDomain(), true);


		// Site colour filter values
		vector<string> domain = Colouring::getSiteColourFilters();
		OptionsAPI::siteColourType = new DiscreteOption("siteColourType", "Alignment", "Which sites should be coloured", Colouring::getDefaultSiteColourFilter(), domain);



		// Get all colour classes
		OptionsAPI::colouringClasses.push_back(new ClustalAmino());
		OptionsAPI::colouringClasses.push_back(new JalView());
		OptionsAPI::colouringClasses.push_back(new AliView());
		OptionsAPI::colouringClasses.push_back(new Drums());

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
	 * Generate all objects. Now ready - to render onto the svg
	 * Return canvas width and height but do not return any of the objects until getGraphics is called
	*/
	void EMSCRIPTEN_KEEPALIVE initGraphics () {


		if (!OptionsAPI::isReady()) {
			WasmAPI::messageFromWasmToJS("");
			return;
		}

		// Clear it so it stops rendering
		OptionsAPI::graphicalObjects = nullptr;


		// Bounds
		double xdivide1 = OptionsAPI::division1->getVal();
		double xdivide2 = OptionsAPI::division2->getVal();

		jsonObject json;
		double width = OptionsAPI::canvasWidth->getVal();
		double height = OptionsAPI::canvasHeight->getVal();

		// Initialise filterings if necessary
		AlignmentAPI::initFiltering(OptionsAPI::variantSitesOnly->getVal(), OptionsAPI::focusOnTaxa->getVal(), (OptionsAPI::focusOnClade->getVal() ? PhylogenyAPI::getTree() : nullptr));

		// Prepare tree-alignment labellings if necessary
		PhylogenyAPI::prepareLabelling(AlignmentAPI::getAlignment());

		// Validate epi mapping
		EpiAPI::validateAccessions(AlignmentAPI::getAlignment());

		// Scroll bars
		jsonObject scrolls;// = new JSONObject();


		// Height of taxa
		double ntHeight = OptionsAPI::siteHeight->getVal();
		cout << "ntHeight " << ntHeight << endl;


		// Full size of view
		double fullHeight = ntHeight * AlignmentAPI::getNtaxaDisplayed() + OptionsAPI::TOP_MARGIN;
		double fullAlnWidth = OptionsAPI::ntWidth->getVal() * (AlignmentAPI::getNsitesDisplayed()+1);



		// Vertical scrolling?
		if (height < fullHeight) {
			Scaling* scaling = new Scaling(0, 0, OptionsAPI::TOP_MARGIN, height);
			scaling->setRowHeight(ntHeight);
			scaling->setScroll(0, OptionsAPI::scrollY->getVal(), 0, fullHeight);


			// Scroll to a taxon?
			if (OptionsAPI::focalTaxon != nullptr) {



				// What row number?
				int rowNum = AlignmentAPI::getTaxonRowNum(OptionsAPI::focalTaxon);

				// Scroll Y value
				scaling->setScroll(0, 0, 0, fullHeight);
				double ypos = scaling->scaleY(rowNum) - OptionsAPI::TOP_MARGIN - height / 2;
				OptionsAPI::scrollY->setVal(ypos / (fullHeight-OptionsAPI::TOP_MARGIN));
				scaling->setScroll(0, OptionsAPI::scrollY->getVal(), 0, fullHeight);

				cout << "Setting scrolly to " << (ypos / (fullHeight-OptionsAPI::TOP_MARGIN)) << " to see " << OptionsAPI::focalTaxon->getName() << endl;

			}


			// Validate scrollY position
			if (OptionsAPI::scrollY->getVal()*height + scaling->getScrollYLength() > height) {
				OptionsAPI::scrollY->setVal((height - scaling->getScrollYLength()) / height);
			}
			scrolls["scrollY"] = OptionsAPI::scrollY->getVal()*height;
			scrolls["scrollYLength"] = scaling->getScrollYLength();
		}else {
			OptionsAPI::canvasHeight->setVal(fullHeight);
			height = OptionsAPI::canvasHeight->getVal();
		}

		// Reset focal taxon
		OptionsAPI::focalTaxon = nullptr;


		// Shrink width?
		if (width*(1-xdivide2) > fullAlnWidth) {
			OptionsAPI::canvasWidth->setVal(width*xdivide2 + fullAlnWidth);
			width = OptionsAPI::canvasWidth->getVal();
			OptionsAPI::division2->setVal((width-fullAlnWidth)/width);
			xdivide2 = OptionsAPI::division2->getVal();
		}



		// Ensure divs are within margins
		if (xdivide1*width < OptionsAPI::LEFT_MARGIN) {
			xdivide1 = OptionsAPI::LEFT_MARGIN / width;
		}
		if (xdivide2*width < OptionsAPI::LEFT_MARGIN) {
			xdivide2 = OptionsAPI::LEFT_MARGIN / width;
		}

		// x-boundary objects
		jsonObject xboundaries;// = new JSONObject();
		xboundaries[OptionsAPI::canvasWidth->getName()] = width;
		xboundaries[OptionsAPI::division1->getName()] = xdivide1*width;
		xboundaries[OptionsAPI::division2->getName()] = xdivide2*width;
		json["xboundaries"] = xboundaries;

		// y-boundary objects
		jsonObject yboundaries;// = new JSONObject();
		yboundaries[OptionsAPI::canvasHeight->getName()] = height;
		json["yboundaries"] = yboundaries;



		// Create graphics
		jsonObject objs = json::array();

		if (PhylogenyAPI::isReady()) {

			PhylogenyAPI::applyFiltering(AlignmentAPI::getFiltering());


			double branchW = OptionsAPI::branchwidth->getVal();
			double nodeRad = OptionsAPI::nodeRadius->getVal();
			double spacing = std::max(OptionsAPI::treeSpacing->getVal(), nodeRad);


			// How tall is the tree?
			double treeHeight = PhylogenyAPI::getHeight();
			double treeMin = 0;
			if (AlignmentAPI::getFiltering()->getSubtreeRoot() != nullptr) {
				treeHeight = AlignmentAPI::getFiltering()->getSubtreeRoot()->getHeight();
				treeMin = AlignmentAPI::getFiltering()->getSubtreeRoot()->getYoungestChildHeight();
			}


			// Scaling
			Scaling* scaling = new Scaling(OptionsAPI::LEFT_MARGIN + spacing,  xdivide1*width - spacing, OptionsAPI::TOP_MARGIN, height, treeHeight, treeMin);
			scaling->setRowHeight(ntHeight);
			scaling->setScroll(0, OptionsAPI::scrollY->getVal(), 0, fullHeight);

			// Annotations
			string internalLabel = OptionsAPI::internalNodeLabels == nullptr ? nullptr : OptionsAPI::internalNodeLabels->getVal();
			string leafLabels = OptionsAPI::leafNodeLabels == nullptr ? nullptr : OptionsAPI::leafNodeLabels->getVal();
			double fontSize = std::min(ntHeight, OptionsAPI::annotationFontSize->getVal());
			int rounding = (int)OptionsAPI::annotationRounding->getVal();

			jsonObject tree = PhylogenyAPI::getTreeGraphics(scaling, branchW, OptionsAPI::showTaxaOnTree->getVal(), nodeRad, internalLabel, leafLabels, fontSize, rounding, OptionsAPI::transmissionTree->getVal());
			//objs.putAll(tree);
			objs.insert(objs.end(), tree.begin(), tree.end()); //test


		}


		// Width of taxa
		if (AlignmentAPI::isReady()) {


			// Taxa
			if (xdivide2 > xdivide1) {
				double x0 = xdivide1*width + OptionsAPI::taxaSpacing->getVal();

				// Scaling
				Scaling* scaling = new Scaling(x0, xdivide2*width, OptionsAPI::TOP_MARGIN, height);
				scaling->setRowHeight(ntHeight);
				scaling->setScroll(0, OptionsAPI::scrollY->getVal(), 0, fullHeight);

				// Font size
				double labelFontSize = std::min(ntHeight, OptionsAPI::fontSizeTaxa->getVal());

				jsonObject taxa = AlignmentAPI::getTaxaGraphics(scaling,  labelFontSize, OptionsAPI::showTaxonNumbers->getVal());
				objs.insert(objs.end(), taxa.begin(), taxa.end()); //test
			}


			// Alignment
			double alnViewWidth = (width - xdivide2*width);

			if (alnViewWidth > 0) {

				double minWidth = OptionsAPI::ntWidth->getVal();
				int nsitesInView = std::ceil(alnViewWidth /  minWidth); // *AlignmentAPI.getNsitesDisplayed();
				Colouring* cols = OptionsAPI::getSelectedColouring();
				cols->setSiteColourFilter(OptionsAPI::siteColourType->getVal(), AlignmentAPI::getFiltering());
				cout << "Using the " << cols->getName() << " scheme" << endl;

				// Scaling
				Scaling* scaling = new Scaling(xdivide2*width, width, OptionsAPI::TOP_MARGIN, height, 0, nsitesInView-1);
				scaling->setRowHeight(ntHeight);
				scaling->setScroll(OptionsAPI::scrollX->getVal(), OptionsAPI::scrollY->getVal(), fullAlnWidth, fullHeight);


				// Font size
				double ntFontSize = std::min(ntHeight, OptionsAPI::fontSizeAln->getVal());

				jsonObject alignment = AlignmentAPI::getAlignmentGraphics(scaling, minWidth, ntFontSize, cols);

				//objs.putAll(alignment);
				objs.insert(objs.end(), alignment.begin(), alignment.end());

				json["nsites"] = AlignmentAPI::getNsites();
				json["nsitesdisplayed"] = AlignmentAPI::getNsitesDisplayed();
				json["ntaxa"] = AlignmentAPI::getNtaxa();
				json["ntaxadisplayed"] = AlignmentAPI::getNtaxaDisplayed();
				json["nuniqueseqs"] = AlignmentAPI::getNumUniqueSequences();


				// Horizontal scrolling?
				if (scaling->getScrollXLength() > 0) {

					// Validate scrollX position
					if (OptionsAPI::scrollX->getVal()*alnViewWidth + scaling->getScrollXLength() > alnViewWidth) {
						OptionsAPI::scrollX->setVal((alnViewWidth - scaling->getScrollXLength()) / alnViewWidth);
					}

					scrolls["scrollX"] = OptionsAPI::scrollX->getVal() * (width - xdivide2*width) + xdivide2*width;
					scrolls["scrollXLength"] = scaling->getScrollXLength();
				}


			}


		}





		// Top and left margin backgrounds
		jsonObject top;
		top["ele"] = "rect";
		top["x"] = 0;
		top["y"] = 0;
		top["width"] = width;
		top["height"] = OptionsAPI::TOP_MARGIN;
		top["fill"] = "#FFDAB9";

		jsonObject left;
		left["ele"] = "rect";
		left["x"] = 0;
		left["y"] = 0;
		left["width"] = OptionsAPI::LEFT_MARGIN;
		left["height"] = height;
		left["fill"] = "#FFDAB9";


		// Add top layer objects
		jsonObject topObjs; // = new JSONArray();
		topObjs.push_back(top);
		topObjs.push_back(left);
		json["objects"] = topObjs;


		OptionsAPI::graphicalObjects = objs;


		json["scrolls"] = scrolls;


		// Return
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
	 * @param goingUp
	*/
	void EMSCRIPTEN_KEEPALIVE scrollABit(int dir) {
		double y1 = OptionsAPI::scrollY->getVal();
		double relativeScrollAmnt = OptionsAPI::SCROLL_Y_NROWS/AlignmentAPI::getNtaxaDisplayed();
		double y2 = y1 + dir*relativeScrollAmnt;
		OptionsAPI::scrollY->setVal(y2);
		//cout << "scrolling " << y1 << " -> " << y2 << " / " << OptionsAPI::scrollY->getVal() << endl;
		WasmAPI::messageFromWasmToJS("");
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
					val = val - (OptionsAPI::canvasWidth->getVal())*OptionsAPI::division2->getVal();
					val = val / (OptionsAPI::canvasWidth->getVal() - (OptionsAPI::canvasWidth->getVal())*OptionsAPI::division2->getVal());
				}

				if (option == OptionsAPI::scrollY) {
					val = val / (OptionsAPI::canvasHeight->getVal());
				}


				v->setVal(val);
			}

			else if (BooleanOption* v = dynamic_cast<BooleanOption*>(option)) {

				bool val = value == "true" || value == "1";

				// Special case: if focusOnClade is enabled, then enable focusOnTaxa too
				if (option == OptionsAPI::focusOnClade && val == true) OptionsAPI::focusOnTaxa->setVal(true);

				// If focusOnTaxa is enabled, then set focusOnClade to false
				if (option == OptionsAPI::focusOnTaxa && val == true) OptionsAPI::focusOnClade->setVal(false);
				if (option == OptionsAPI::focusOnClade || option == OptionsAPI::focusOnTaxa) {
					OptionsAPI::resetScroll();
					AlignmentAPI::setSelectionToDirty();
				}

				cout << "setting " << option->getName() << " to " << val << "|" << value << endl;

				v->setVal(val);
			}

			else if (DiscreteOption* v = dynamic_cast<DiscreteOption*>(option)) {
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


		// Set it as the focal taxon
		OptionsAPI::focalTaxon = taxon;


		WasmAPI::messageFromWasmToJS("");



	}




	/**
	 * Is the system ready to render?
	 * @return
	 */
	void EMSCRIPTEN_KEEPALIVE isReady() {
		jsonObject json;
		json["ready"] = OptionsAPI::isReady();
		cout << "checking for readiness" << endl;
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











}












