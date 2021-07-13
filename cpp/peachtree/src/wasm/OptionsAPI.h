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
	static DiscreteOption* internalNodeLabels;
	static DiscreteOption* leafNodeLabels;
	static NumericalOption* annotationFontSize;
	static NumericalOption* annotationRounding;

	// Taxa
	static NumericalOption* siteHeight;
	static NumericalOption* fontSizeTaxa;
	static NumericalOption* taxaSpacing;
	static BooleanOption* showTaxonNumbers;
	static BooleanOption* focusOnTaxa;
	static BooleanOption* focusOnClade;

	// Alignment
	static NumericalOption* ntWidth;
	static NumericalOption* fontSizeAln;
	static BooleanOption* variantSitesOnly;
	static DiscreteOption* siteColourType;
	static DiscreteOption* colourings;


	// Epidemiology
	static DiscreteOption* epiSymptomDate;
	static NumericalOption* infectiousPeriodBefore;
	static NumericalOption* infectiousPeriodAfter;



	// Methods
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



	// Constants
	static const long CHUNK_SIZE;
	static const double LEFT_MARGIN; // Left hand margin
	static const double TOP_MARGIN; // Top margin
	static const double INIT_WIDTH;
	static const double INIT_HEIGHT;
	static const double INIT_DIV1;
	static const double INIT_DIV2;
	static const double SCROLL_Y_NROWS;


	// Variables
	static jsonObject graphicalObjects;
	static Taxon* focalTaxon;
	static vector<Colouring*> colouringClasses;


private:




};

#endif /* WASM_OPTIONSAPI_H_ */