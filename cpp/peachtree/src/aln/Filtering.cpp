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

Filtering::Filtering(bool variantSitesOnly, bool focus, Alignment* alignment, Tree* tree) {


	cout << "...preparing filtering..." << endl;
	this->alignment = alignment;
	this->focusing = focus;
	this->variantSitesOnly = variantSitesOnly;
	this->variantSitesOnly_parsed = variantSitesOnly;
	this->isNucleotide = alignment->getIsNucleotide();
	this->subtree = nullptr;
	this->numUniqueSeqs = -1;

	// Use unique taxa ids
	this->taxaIDsToInclude.clear();



	// Use the tree to find taxa
	if (tree != nullptr) {

		this->tree = tree;

		// Find selected taxa
		vector<Taxon*> selected;
		for (Sequence* seq : alignment->getSequences()) {
			Taxon* taxon = seq->getTaxon();
			if (taxon->getIsSelected()) selected.push_back(taxon);
		}


		// Find their mrca and take the full clade
		selected = tree->getClade(selected);
		this->subtree = tree->getMRCA(selected);


		// Select the ones to include
		for (Taxon* taxon : selected) {
			taxon->setIsSelected(true);
		}

	}



	// Use all taxa / selected taxa
	for (Sequence* seq : alignment->getSequences()) {
		Taxon* taxon = seq->getTaxon();
		if (!focus || taxon->getIsSelected()) {
			this->taxaIDsToInclude[taxon->getID()] = true;
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
		}
		numTaxa = taxaIDsToInclude.size();
	}

	this->majors.clear();





	// Which sites to include?
	if (this->variantSitesOnly) {
		for (int site = 0; site < alignment->getLength(); site ++) {

			bool includeSite = false;
			int uniqueSymbol = -1;
			for (int taxa = 0; taxa < alignment->getNtaxa(); taxa++) {

				Taxon* taxon = alignment->getSequence(taxa)->getTaxon();

				// Should this taxon be included?
				if (!this->includeTaxon(taxon)) continue;

				Sequence* sequence = alignment->getSequence(taxa);

				// What is the symbol?
				int symbol = sequence->getSymbolInt(site);
				if (Alignment::isAmbiguousOrGap(symbol, sequence->getIsNucleotide())) continue;

				if (uniqueSymbol == -1) {
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




	if (numSites == 0) this->variantSitesOnly = false;

	if (!this->variantSitesOnly) {
		numSites = alignment->getLength();
		for (int i = 0; i < numSites; i ++) {
			this->sitesToIncludeList.push_back(i);
		}
	}

	// Prepare major/minor alleles
	this->prepareMajorAlleles(this->alignment);
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
	if (this->subtree != nullptr) {
		this->subtree->cleanup();
		delete this->subtree;
	}
	this->taxaIDsToInclude.clear();
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
	Sequence* copy = seqsToInclude.at(0)->copy();
	seqClasses.push_back(copy);


	for (int i = 1; i < seqsToInclude.size(); i ++) {

		Sequence* sequence = seqsToInclude.at(i);

		// Match it to other classes?
		bool foundMatch = false;
		for (Sequence* refseq : seqClasses) {


			if (this->numTaxa == 4) cout << "Comparing " << sequence->getAcc() << " with " << refseq->getAcc();


			bool refSeqMatch = true;
			for (int siteNum : this->sitesToIncludeList) {

				int char1 = sequence->getSymbolInt(siteNum);
				int char2 = refseq->getSymbolInt(siteNum);

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


				// Fill in all ambiguous sites in the ref seq with those in this seq
				for (int siteNum : this->sitesToIncludeList) {


					int char1 = sequence->getSymbolInt(siteNum);
					int char2 = refseq->getSymbolInt(siteNum);
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
		}


	}

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
 */
void Filtering::prepareMajorAlleles(Alignment* alignment){


	cout << "Finding major alleles" << endl;

	this->majors.clear();
	if (numTaxa < 2 || this->sitesToIncludeList.empty()) return;
	std::map<int, int> freqs;
	int siteNum, count, character;
	for (int s = 0; s < this->numSites; s ++) {


		siteNum = this->sitesToIncludeList.at(s);

		// List all characters by frequency
		freqs.clear();
		for (const auto &pair : this->taxaIDsToInclude) {

			int taxonNum = pair.first;

			// Count
			character = alignment->getSequence(taxonNum)->getSymbolInt(siteNum);
			if (character == -1) continue;
			if (freqs.count(character) > 0) {
				count = freqs[character] + 1;
			}else {
				count = 1;
			}
			freqs[character] = count;

		}


		// Find major allele at this site
		int maxCount = 0;
		int major = -1;
		for (const auto &pair : freqs) {
			int c = pair.first;
			count = freqs[c];
			if (count > maxCount) {
				maxCount = count;
				major = c;
			}
		}

		this->majors[siteNum] = major;

	}


}














