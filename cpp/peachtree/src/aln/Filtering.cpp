/*
 * Filtering.cpp
 *
 *  Created on: 30/06/2021
 *      Author: jdou557
 */

#include "Filtering.h"
#include "Alignment.h"
#include "../phy/Tree.h"
#include "../phy/Node.h"
#include "Taxon.h"
#include <vector>

Filtering::Filtering(bool variantSitesOnly, bool focus,  bool focusOnClade, Alignment* alignment, Tree* tree) {
	this->init(variantSitesOnly, focus, focusOnClade, alignment, tree);
}

void Filtering::init(bool variantSitesOnly, bool focus,  bool focusOnClade, Alignment* alignment, Tree* tree) {
	
	this->cleanup();

	

	cout << "...preparing filtering..." << endl;
	this->alignment = alignment;
	this->focusing = focus;
	this->focusingOnClade = focusOnClade;
	this->variantSitesOnly = variantSitesOnly;
	this->variantSitesOnly_parsed = variantSitesOnly;
	this->isNucleotide = alignment->getIsNucleotide();
	this->subtree = nullptr;
	this->numUniqueSeqs = -1;




	// Use the tree to find taxa
	if (tree != nullptr) {

		this->tree = tree;



		// Find selected taxa
		vector<Taxon*> selected;
		for (Sequence* seq : alignment->getSequences()) {
			Taxon* taxon = seq->getTaxon();
			if (taxon->getIsSelected()) selected.push_back(taxon);
		}
		

		
		
		
		
		if (focusOnClade) {
			
			this->subtree = tree->getMRCA(selected);
			
			// Find their mrca and take the full clade
			vector<Taxon*> selectedClade = tree->getClade(selected);

			// Select the ones to include
			for (Taxon* taxon : selectedClade) {
				taxon->setIsSelected(true);
			}
		
		}else if (focusing){
			
			
			this->subtree = tree->getSubtree(selected);
			
			vector<Node*> allNodes = Tree::listNodes(subtree);
			
			int nr = 0;
			for (int i = 0; i < subtree->getLeafNodeCount(); i ++){

				Node* leaf = allNodes.at(i);

				// Maintain a separate numbering system where filtered-out leaves are omitted
				if (this->includeTaxon(leaf->getTaxon())) {
					leaf->setFilteredNr(nr);
					nr ++;
				}else {
					leaf->setFilteredNr(-1);
				}
			}
			
		}
			

	}
	
	



	// Use all taxa / selected taxa
	for (Sequence* seq : alignment->getSequences()) {
		Taxon* taxon = seq->getTaxon();
		if (!focus || taxon->getIsSelected()) {
			this->taxaIDsToInclude[taxon->getID()] = true;
			this->taxonIDsIncludeList.push_back(taxon->getID());
			//taxaIDsToInclude.put(taxon.getID(), true);
		}
	}
	numTaxa = taxaIDsToInclude.size();

	// Just show everything
	if (numTaxa == 0) {
		for (Sequence* seq : alignment->getSequences()) {
			Taxon* taxon = seq->getTaxon();
			//taxaIDsToInclude.put(taxon.getID(), true);
			this->taxaIDsToInclude[taxon->getID()] = true;
			this->taxonIDsIncludeList.push_back(taxon->getID());
		}
		numTaxa = taxaIDsToInclude.size();
	}
	
		



	// Which sites to include?
	if (this->variantSitesOnly) {
		for (int site = 0; site < alignment->getLength(); site ++) {
			
			
			if (alignment->isConstantSite(site)) continue;

			bool includeSite = false;
			char uniqueSymbol = '?';
			for (int taxa = 0; taxa < alignment->getNtaxa(); taxa++) {

				Taxon* taxon = alignment->getSequence(taxa)->getTaxon();

				// Should this taxon be included?
				if (!this->includeTaxon(taxon)) continue;

				Sequence* sequence = alignment->getSequence(taxa);

				// What is the symbol?
				char symbol = sequence->getSymbolChar(site);
				//if (Alignment::isAmbiguousOrGap(symbol, sequence->getIsNucleotide())) continue;
				if (Alignment::isAmbiguous(symbol, sequence->getIsNucleotide())) continue;

				if (uniqueSymbol == '?') {
					uniqueSymbol = symbol;
				}
				else if (uniqueSymbol != symbol) {
					includeSite = true;
					break;
				}

			}


			// Include the site
			if (includeSite) {
				this->sitesToIncludeMap[site] = true;
				this->sitesToIncludeList.push_back(site);
			}


		}

		numSites = sitesToIncludeMap.size();

	}



	//if (numSites == 0) this->variantSitesOnly = false;

	if (!this->variantSitesOnly) {
		numSites = alignment->getLength();
		for (int i = 0; i < numSites; i ++) {
			this->sitesToIncludeList.push_back(i);
		}
	}
	
	
	// Prepare major/minor alleles
	this->prepareMajorAlleles(this->alignment);



}



/*
 * Update the ordering of the taxa based on the new alignment ordering
 *
 */
void Filtering::updateOrdering(Alignment* alignment){
	this->taxonIDsIncludeList.clear();
	
	for (Sequence* seq : alignment->getSequences()) {
		Taxon* taxon = seq->getTaxon();
		//taxaIDsToInclude.put(taxon.getID(), true);
		if (this->taxaIDsToInclude.count(taxon->getID()) > 0){
			this->taxonIDsIncludeList.push_back(taxon->getID());
		}			
		
	}
	
}


/**
 * Should this taxon be included?
 * @param acc
 * @return
 */
bool Filtering::includeTaxon(Taxon* taxon){
	if (this->taxaIDsToInclude.empty()) return true;
	return this->taxaIDsToInclude.count(taxon->getID()) > 0;
}



/**
 * Get the display index of the taxon by its ID
 *
 */
int Filtering::getIndexByTaxonID(int taxonID){
	
	if (taxaIDsToInclude.empty()) return -1;
	
	int index = 0;
	for (int t : this->taxonIDsIncludeList) {
		if (t == taxonID) return index;
		index++;
	}
	
	return -1;
	
}


/**
 * Get the taxon by its display index
 *
 */
int Filtering::getTaxonIDByIndex(int index){
	
	if (index < 0 || index >= this->taxonIDsIncludeList.size()) return -1;
	return this->taxonIDsIncludeList.at(index);

	
	
}



/**
 * Return the root of the subsetted subtree, if applicable
 * @return
 */
Node* Filtering::getSubtreeRoot(){
	return this->subtree;
}


/**
 * Are variant sites only being displayed?
 * @return
 */
bool Filtering::getVariantSitesOnly(){
	return this->variantSitesOnly;
}

bool Filtering::variantSitesOnlyParsed(){
	return this->variantSitesOnly_parsed;
}

Tree* Filtering::getTree(){
	return this->tree;
}


/**
 * Are the selected sites the only ones being displayed?
 * @return
 */
bool Filtering::getFocusing(){
	return this->focusing;
}

bool Filtering::getFocusingOnClade(){
	return this->focusingOnClade;
}


/**
 * Should this site number be included
 * @param site
 * @return
 */
bool Filtering::includeSite(int site){
	if (!this->variantSitesOnly) return true;
	if (this->sitesToIncludeMap.empty()) return true;
	return this->sitesToIncludeMap.count(site) > 0;
}


vector<int> Filtering::getSites(){
	return this->sitesToIncludeList;
}



void Filtering::cleanup(){
	this->alignment = nullptr;
	this->subtree = nullptr;
	this->tree = nullptr;
	this->taxaIDsToInclude.clear();
	this->taxonIDsIncludeList.clear();
	this->sitesToIncludeMap.clear();
	this->sitesToIncludeList.clear();
	this->majors.clear();
}


/**
 * Number of sites included by filter
 * @return
 */
int Filtering::getNumSites(){
	return this->numSites;
}


/**
 * Number of sequences included by filter
 * @return
 */
int Filtering::getNumSeqs(){
	return this->numTaxa;
}


/**
 * Is this character at this site the major allele?
 * @param character
 * @param siteNum
 * @return
 */
bool Filtering::isMajorAllele(string character, int siteNum){
	return this->isMajorOrMinorAllele(character, siteNum, true);
}

/**
 * Is this character at this site a non-major allele?
 * @param character
 * @param siteNum
 * @return
 */
bool Filtering::isMinorAllele(string character, int siteNum){
	return this->isMajorOrMinorAllele(character, siteNum, false);
}

/**
 * Get the index of this taxon
 * @param taxon
 * @return
 */
int Filtering::getIndex(Taxon* taxon){
	int rowNum = 0;
	for (const auto &pair : this->taxaIDsToInclude) {
		int taxonID = pair.first;
		if (taxon->getID() == taxonID) return rowNum;
		rowNum ++;
	}
	return -1;
}

/**
 * Count the number of unique sequences in the alignment
 * Ambiguous characters don't affect the count
 * @return
 */
int Filtering::getNumUniqueSequences(){


	if (this->numTaxa == 0) return 0;
	if (this->numTaxa == 1) return 1;
	if (this->numUniqueSeqs >= 0) return this->numUniqueSeqs;


	// Get list of sequences
	vector<Sequence*> seqsToInclude;
	Alignment* aln = this->alignment;
	for (Sequence* seq : aln->getSequences()) {
		if (this->includeTaxon(seq->getTaxon())) {
			seqsToInclude.push_back(seq);
		}
	}



	// Create a list of sequence classes and try to match each subsequent sequence to a class
	vector<Sequence*> seqClasses;
	vector<int> classSizes;
	Sequence* copy = seqsToInclude.at(0)->copy();
	seqClasses.push_back(copy);
	classSizes.push_back(1);


	for (int i = 1; i < seqsToInclude.size(); i ++) {

		Sequence* sequence = seqsToInclude.at(i);

		// Match it to other classes?
		bool foundMatch = false;
		for (int refseqNum = 0; refseqNum < seqClasses.size(); refseqNum++) {
			
			Sequence* refseq = seqClasses.at(refseqNum);

			if (this->numTaxa == 4) cout << "Comparing " << sequence->getAcc() << " with " << refseq->getAcc();


			bool refSeqMatch = true;
			for (int siteNum : this->sitesToIncludeList) {

				char char1 = sequence->getSymbolChar(siteNum);
				char char2 = refseq->getSymbolChar(siteNum);

				// If the two symbols are identical, or one is ambiguous, then move on to next site
				if (char1 == char2) continue;
				if (Alignment::isAmbiguous(char1, this->isNucleotide)) continue;
				if (Alignment::isAmbiguous(char2, this->isNucleotide)) continue;


				// Otherwise the match failed
				refSeqMatch = false;
				break;

			}


			if (refSeqMatch) {
				foundMatch = true;
				classSizes.at(refseqNum) = classSizes.at(refseqNum) + 1;

				// Fill in all ambiguous sites in the ref seq with those in this seq
				for (int siteNum : this->sitesToIncludeList) {


					char char1 = sequence->getSymbolChar(siteNum);
					char char2 = refseq->getSymbolChar(siteNum);
					if (Alignment::isAmbiguous(char2, this->isNucleotide) && !Alignment::isAmbiguous(char1, this->isNucleotide)) {
						refseq->editSiteInt(siteNum, char1);
					}

				}
				
				break;
			}


		}

		if (!foundMatch) {
			Sequence* copy = sequence->copy();
			seqClasses.push_back(copy);
			classSizes.push_back(1);
		}


	}
	
	
	
	// What is the largest class size?
	int largest = 0;
	for (int size : classSizes){
		largest = std::max(largest, size);
	}
	cout << "The largest seq class has " << largest << " sequences" << endl;
	

	this->numUniqueSeqs = seqClasses.size();
	return this->numUniqueSeqs;


}

/**
 * Is this character at this site a major allele (if isMajor=true) or a minor allele (if false)
 * @param character
 * @param siteNum
 * @param isMajor
 * @return
 */
bool Filtering::isMajorOrMinorAllele(string character, int siteNum, bool isMajor){


	if (this->numTaxa < 2) {
		return isMajor;
	}
	if (!(this->majors.count(siteNum) > 0)) return false;

	// Convert to int
	int charInt;
	if (this->isNucleotide) {
		charInt = Alignment::getNucleotideInt(character);
	}else {
		charInt = Alignment::getAlphaInt(character);
	}

	int major = this->majors.at(siteNum);
	if (isMajor) return major == charInt;
	return major != charInt;


}


/**
 * Prepare the arrays of major alleles at each site
 * Can be very slow to run
 */
void Filtering::prepareMajorAlleles(Alignment* alignment){


	cout << "Finding major alleles " << this->numSites << "/" << this->taxaIDsToInclude.size() << endl;
	
	vector<char> validSymbols = Alignment::getSymbols(alignment->getIsNucleotide());

	this->majors.clear();
	if (numTaxa < 2 || this->sitesToIncludeList.empty()) return;
	//std::map<char, int> freqs;
	vector<int> freqs(validSymbols.size());
	int siteNum, count, maxCount, symbolNum, major, c;
	int character;
	
	
	// Build list of taxon indices
	vector<int> taxonIndices;
	for (const auto &pair : this->taxaIDsToInclude) {
		int taxonID = pair.first;
		int taxonIndex = alignment->getTaxonIndex(taxonID);
		taxonIndices.push_back(taxonIndex);
	}
	int halfWay = taxonIndices.size() / 2 + 1;
	
	
	for (int s = 0; s < this->numSites; s ++) {


		siteNum = this->sitesToIncludeList.at(s);

		// List all characters by frequency
		//freqs.clear();
		
		// Reset freq map
		for (symbolNum = 0; symbolNum < freqs.size(); symbolNum++){
			freqs.at(symbolNum) = 0;
		}
		
		maxCount = 0;
		
		for (int i = 0; i < taxonIndices.size(); i ++) {

			
			// Count
			character = alignment->getSequence(taxonIndices.at(i))->getSymbolInt(siteNum);
			
			if (Alignment::isAmbiguousOrGap(character, alignment->getIsNucleotide())) continue;
			
			
			count = freqs.at(character) + 1;
			freqs.at(character) = count;
			
			
			// If a count exceeds 50%, we can stop now (for efficiency)
			if (count > maxCount) {
				maxCount = count;
				if (maxCount >= halfWay){
					break;
				}
			}

		}


		// Find major allele at this site
		maxCount = 0;
		major = -1;
		for (c = 0; c < freqs.size(); c++){
			count = freqs.at(c);
			//cout << "site " << siteNum << ": count of " << c << " is " << count << endl;
			if (count > maxCount) {
				maxCount = count;
				major = c;
			}
		}

		//cout << "major at " << siteNum << " is " << major << " with count " << maxCount << endl;
		this->majors[siteNum] = major;

	}
	


}














