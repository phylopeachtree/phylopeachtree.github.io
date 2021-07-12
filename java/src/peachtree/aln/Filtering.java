package peachtree.aln;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import peachtree.phy.Node;
import peachtree.phy.Tree;


/**
 * Filtering of sites and taxa
 * @author Jordan
 *
 */
public class Filtering {

	
	boolean variantSitesOnly;
	boolean variantSitesOnly_parsed; // Value parsed in construction
	boolean focusing;
	Map<Integer, Boolean> taxaIDsToInclude;
	Map<Integer, Boolean> sitesToIncludeMap;
	List<Integer> sitesToIncludeList;
	Alignment alignment;
	int numUniqueSeqs;
	
	
	// Major characters at each site
	Map<Integer, Integer> majors;
	Tree tree;
	
	int numSites;
	int numTaxa;
	boolean isNucleotide;
	Node subtree; // The subtree (if using a tree as a filter)
	
	public Filtering(boolean variantSitesOnly, boolean focus, Alignment alignment, Tree tree) {
		
		
		System.out.println("...preparing filtering...");
		this.alignment = alignment;
		this.focusing = focus;
		this.variantSitesOnly = variantSitesOnly;
		this.variantSitesOnly_parsed = variantSitesOnly;
		this.isNucleotide = alignment.isNucleotide;
		this.subtree = null;
		this.numUniqueSeqs = -1;
		
		// Use unique taxa ids
		this.taxaIDsToInclude = new LinkedHashMap<>();
		
		
		// Use the tree to find taxa
		if (tree != null) {
			
			this.tree = tree;
			
			// Find selected taxa
			List<Taxon> selected = new ArrayList<>();
			for (Sequence seq : alignment.getSequences()) {
				Taxon taxon = seq.getTaxon();
				if (taxon.isSelected()) selected.add(taxon);
			}
			
			
			// Find their mrca and take the full clade
			selected = tree.getClade(selected);
			this.subtree = tree.getMRCA(selected);
			
			
			// Select the ones to include
			for (Taxon taxon : selected) {
				taxon.isSelected(true);
			}
			
		}
			
			
			
		// Use all taxa / selected taxa
		for (Sequence seq : alignment.getSequences()) {
			Taxon taxon = seq.getTaxon();
			if (!focus || taxon.isSelected()) taxaIDsToInclude.put(taxon.getID(), true);
		}
		numTaxa = taxaIDsToInclude.size();
		
		// Just show everything
		if (numTaxa == 0) {
			for (Sequence seq : alignment.getSequences()) {
				Taxon taxon = seq.getTaxon();
				taxaIDsToInclude.put(taxon.getID(), true);
			}
			numTaxa = taxaIDsToInclude.size();
		}
		
		this.majors = null;
		

		// Which sites to include?
		this.sitesToIncludeMap = new LinkedHashMap<>();
		this.sitesToIncludeList = new ArrayList<>();
		if (this.variantSitesOnly) {
			for (int site = 0; site < alignment.getLength(); site ++) {
				
				boolean includeSite = false;
				int uniqueSymbol = -1;
				for (int taxa = 0; taxa < alignment.getNtaxa(); taxa++) {
					
					Taxon taxon = alignment.getSequence(taxa).getTaxon();
					
					// Should this taxon be included?
					if (!this.includeTaxon(taxon)) continue;
					
					Sequence sequence = alignment.getSequence(taxa);
					
					// What is the symbol?
					int symbol = sequence.getSymbolInt(site);
					if (Alignment.isAmbiguousOrGap(symbol, sequence.isNucleotide())) continue;
					
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
					this.sitesToIncludeMap.put(site, true);
					this.sitesToIncludeList.add(site);
				}
				
				
				
				
			}
			
			numSites = sitesToIncludeMap.size();
		
		}
		
		if (numSites == 0) this.variantSitesOnly = false;
		
		if (!this.variantSitesOnly) {
			numSites = alignment.getLength();
			for (int i = 0; i < numSites; i ++) {
				this.sitesToIncludeList.add(i);
			}
		}
		
		
		
		// Prepare major/minor alleles
		this.prepareMajorAlleles(alignment);
		
	}
	
	
	/**
	 * Should this taxon be included?
	 * @param acc
	 * @return
	 */
	public boolean includeTaxon(Taxon taxon) {
		if (this.taxaIDsToInclude == null) return true;
		if (this.taxaIDsToInclude.isEmpty()) return true;
		return this.taxaIDsToInclude.containsKey(taxon.getID());
		
	}
	
	
	/**
	 * Return the root of the subsetted subtree, if applicable
	 * @return
	 */
	public Node getSubtreeRoot() {
		return this.subtree;
	}
	
	
	/**
	 * Are variant sites only being displayed?
	 * @return
	 */
	public boolean variantSitesOnly() {
		return this.variantSitesOnly;
	}
	
	
	public boolean variantSitesOnlyParsed() {
		return this.variantSitesOnly_parsed;
	}
	
	
	
	public Tree getTree() {
		return this.tree;
	}
	
	/**
	 * Are the selected sites the only ones being displayed?
	 * @return
	 */
	public boolean focusing() {
		return this.focusing;
	}
	
	
	/**
	 * Should this site number be included
	 * @param site
	 * @return
	 */
	public boolean includeSite(int site) {
		if (!this.variantSitesOnly) return true;
		if (this.sitesToIncludeMap == null) return true;
		if (this.sitesToIncludeMap.isEmpty()) return true;
		return this.sitesToIncludeMap.containsKey(site);
	}
	
	
	public List<Integer> getSites() {
		return this.sitesToIncludeList;
	}


	/**
	 * Number of sites included by filter
	 * @return
	 */
	public int getNumSites() {
		return numSites;
	}


	/**
	 * Number of sequences included by filter
	 * @return
	 */
	public int getNumSeqs() {
		return numTaxa;
	}

	
	
	/**
	 * Prepare the arrays of major alleles at each site
	 */
	private void prepareMajorAlleles(Alignment alignment) {
		
		
		this.majors = new HashMap<>();
		if (numTaxa < 2 || this.sitesToIncludeList.isEmpty()) return;
		HashMap<Integer, Integer> freqs = new HashMap<>();
		int siteNum, count, character;
		for (int s = 0; s < this.numSites; s ++) {
			
			
			siteNum = this.sitesToIncludeList.get(s);
			
			// List all characters by frequency
			freqs.clear();
			for (int taxonNum : this.taxaIDsToInclude.keySet()) {
				
				// Count
				character = alignment.getSequence(taxonNum).getSymbolInt(siteNum);
				if (freqs.containsKey(character)) {
					count = freqs.get(character) + 1;
				}else {
					count = 1;
				}
				freqs.put(character, count);
				
			}
			
			
			// Find major allele at this site
			int maxCount = 0;
			int major = -1;
			for (Integer c : freqs.keySet()) {
				count = freqs.get(c);
				if (count > maxCount) {
					maxCount = count;
					major = c;
				}
			}
			this.majors.put(siteNum, major);
			
			
		}
		
	}
	
	
	/**
	 * Is this character at this site the major allele?
	 * @param character
	 * @param siteNum
	 * @return
	 */
	public boolean isMajorAllele(String character, int siteNum) {
		return isMajorOrMinorAllele(character, siteNum, true);
	}

	
	/**
	 * Is this character at this site a non-major allele?
	 * @param character
	 * @param siteNum
	 * @return
	 */
	public boolean isMinorAllele(String character, int siteNum) {
		return isMajorOrMinorAllele(character, siteNum, false);
	}
	
	
	/**
	 * Is this character at this site a major allele (if isMajor=true) or a minor allele (if false)
	 * @param character
	 * @param siteNum
	 * @param isMajor
	 * @return
	 */
	private boolean isMajorOrMinorAllele(String character, int siteNum, boolean isMajor) {
		
		if (this.numTaxa < 2) {
			return isMajor;
		}
		if (!this.majors.containsKey(siteNum)) return false;
		
		// Convert to int
		int charInt;
		if (this.isNucleotide) {
			charInt = Alignment.nt_chars.get(character);
		}else {
			charInt = Alignment.alpha_chars.get(character);
		}
		
		int major = this.majors.get(siteNum);
		if (isMajor) return major == charInt;
		return major != charInt;
		
	}


	/**
	 * Get the index of this taxon
	 * @param taxon
	 * @return
	 */
	public int getIndex(Taxon taxon) {
		
		int rowNum = 0;
		for (int taxonID : this.taxaIDsToInclude.keySet()) {
			if (taxon.getID() == taxonID) return rowNum;
			rowNum ++;
		}
		return -1;
	}
	
	
	
	/**
	 * Count the number of unique sequences in the alignment
	 * Ambiguous characters don't affect the count
	 * @return
	 */
	public int getNumUniqueSequences() {
		
		if (this.numTaxa == 0) return 0;
		if (this.numTaxa == 1) return 1;
		if (this.numUniqueSeqs >= 0) return this.numUniqueSeqs;
		
		
		// Get list of sequences
		List<Sequence> seqsToInclude = new ArrayList<>();
		for (Sequence seq : this.alignment.getSequences()) {
			if (this.includeTaxon(seq.getTaxon())) {
				seqsToInclude.add(seq);
			}
		}
		
		
		
		// Create a list of sequence classes and try to match each subsequent sequence to a class
		List<Sequence> seqClasses = new ArrayList<>();
		seqClasses.add(seqsToInclude.get(0).copy());
		
		
		for (int i = 1; i < seqsToInclude.size(); i ++) {
			
			Sequence sequence = seqsToInclude.get(i);
			
			// Match it to other classes?
			boolean foundMatch = false;
			for (Sequence refseq : seqClasses) {
				
				
				if (this.numTaxa == 4) System.out.println("Comparing " + sequence.getAcc() + " with " + refseq.getAcc());
				
				
				boolean refSeqMatch = true;
				for (int siteNum : this.sitesToIncludeList) {
					
					int char1 = sequence.getSymbolInt(siteNum);
					int char2 = refseq.getSymbolInt(siteNum);
					
					// If the two symbols are identical, or one is ambiguous, then move on to next site
					if (char1 == char2) continue;
					if (Alignment.isAmbiguous(char1, this.isNucleotide)) continue;
					if (Alignment.isAmbiguous(char2, this.isNucleotide)) continue;
					
					
					// Otherwise the match failed
					refSeqMatch = false;
					break;
					
				}
				
				
				if (refSeqMatch) {
					foundMatch = true;
					
					
					// Fill in all ambiguous sites in the ref seq with those in this seq
					for (int siteNum : this.sitesToIncludeList) {
						
						
						int char1 = sequence.getSymbolInt(siteNum);
						int char2 = refseq.getSymbolInt(siteNum);
						if (Alignment.isAmbiguous(char2, this.isNucleotide) && !Alignment.isAmbiguous(char1, this.isNucleotide)) {
							refseq.editSiteInt(siteNum, char1);
						}
						
					}
					
					break;
				}
				
				
			}
			
			if (!foundMatch) {
				seqClasses.add(sequence.copy());
			}
			
			
		}
		
		this.numUniqueSeqs = seqClasses.size();
		return this.numUniqueSeqs;
	}



	
	
}















