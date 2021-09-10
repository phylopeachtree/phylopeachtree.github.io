/*
 * OptionsAPI.h
 *
 *  Created on: 7/07/2021
 *      Author: jdou557
 */


#include <iostream>
#include "../lib/json.hpp"
#include "../aln/Taxon.h"
#include "../aln/colourings/Colouring.h"
#include "../options/Option.h"
#include "../options/NumericalOption.h"
#include "../options/BooleanOption.h"
#include "../options/DiscreteOption.h"
#include "../options/ColourOption.h"


using jsonObject = nlohmann::json;
using namespace std;

#ifndef WASM_OPTIONSAPI_H_
#define WASM_OPTIONSAPI_H_

class OptionsAPI {
public:

	// Boundaries
	static NumericalOption* canvasWidth;
	static NumericalOption* canvasHeight;
	static NumericalOption* division1;
	static NumericalOption* division2;


	// Scroll bars
	static NumericalOption* scrollY;
	static NumericalOption* scrollX;


	// Phylogeny
	static DiscreteOption* treeMethods;
	static NumericalOption* branchwidth;
	static NumericalOption* nodeRadius;
	static NumericalOption* treeSpacing;
	static BooleanOption* showTaxaOnTree;
	static BooleanOption* transmissionTree;
	//static DiscreteOption* internalNodeLabels;
	//static DiscreteOption* leafNodeLabels;
	static NumericalOption* annotationFontSize;
	static NumericalOption* annotationRounding;
	static ColourOption* branchColouring;
	static DiscreteOption* colourBranchesBy;
	static ColourOption* nodeColouring;
	static DiscreteOption* colourNodesBy;

	// Taxa
	static NumericalOption* zoomScale;
	//static NumericalOption* siteHeight;
	//static NumericalOption* fontSizeTaxa;
	static NumericalOption* taxaSpacing;
	static BooleanOption* showTaxonNumbers;
	static BooleanOption* focusOnTaxa;
	static BooleanOption* focusOnClade;
	static DiscreteOption* sampleNameAnnotation;
	

	// Alignment
	//static NumericalOption* ntWidth;
	//static NumericalOption* fontSizeAln;
	static BooleanOption* variantSitesOnly;
	static BooleanOption* displayMissingPercentage;
	static DiscreteOption* siteColourType;
	static DiscreteOption* colourings;
	
	


	// Epidemiology
	static DiscreteOption* epiSampleDate;
	static DiscreteOption* epiSymptomDate;
	static DiscreteOption* epiIsolationDate;
	static NumericalOption* infectiousPeriodBefore;
	static NumericalOption* infectiousPeriodAfter;
	static DiscreteOption* dateFormat;
	static NumericalOption* timelineFontSize;
	static BooleanOption* displayIncompatibleTranmissions;
	static BooleanOption* reportNumberOfInfections;


	// Methods
	static void init();
	static jsonObject initGraphics(double maxH, double maxW, int downloadInt);
	static bool isReady();
	static bool getVariantSitesOnly();
	static void prepareColourings();
	static void resetScroll();
	static void resetWindowSize();
	static vector<Option*> getOptionList();
	static Colouring* getSelectedColouring();
	static void prepareTreeAnnotationOptions();
	static bool getFocusingOnTaxa();
	static bool getFocusOnClade();
	static void setFocusingOnTaxa(bool val);
	static void setFocusOnClade(bool val);
	static void prepareEpiAnnotations();
	static void reorderTree();



	// Constants
	static const long CHUNK_SIZE;
	static const double LEFT_MARGIN; // Left hand margin
	static double TOP_MARGIN; // Top margin
	static const double MARGIN_SIZE; // Left/top margin with background
	static const double INIT_WIDTH;
	static const double INIT_HEIGHT;
	static const double INIT_DIV1;
	static const double INIT_DIV2;
	static const double SCROLL_Y_NROWS;
	static const int SITE_NUMBERING_EVERY;
	
	static const double NT_HEIGHT;
	static const double NT_WIDTH;
	static const double FONT_SIZE_ALN;
	static const double FONT_SIZE_TAXA;
	
	static const double TREE_LADDER_WIDTH;

	// Variables
	static jsonObject graphicalObjects;
	static Taxon* focalTaxon;
	static vector<Colouring*> colouringClasses;


private:



};

#endif /* WASM_OPTIONSAPI_H_ */

