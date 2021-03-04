package peachtree.aln;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


/**
 * Filtering of sites and taxa
 * @author Jordan
 *
 */
public class Filtering {

	
	boolean variantSitesOnly;
	Map<Integer, Boolean> taxaIDsToInclude;
	Map<Integer, Boolean> sitesToIncludeMap;
	List<Integer> sitesToIncludeList;
	
	
	// Major characters at each site
	Map<Integer, Integer> majors;
	
	
	int numSites;
	int numTaxa;
	boolean isNucleotide;
	
	public Filtering(boolean variantSitesOnly, List<Taxon> taxaToInclude, Alignment alignment) {
		this.variantSitesOnly = variantSitesOnly;
		this.isNucleotide = alignment.isNucleotide;
		
		
		// Use unique taxa ids
		this.taxaIDsToInclude = new HashMap<>();
		if (taxaToInclude != null) {
			for (Taxon taxon: taxaToInclude) {
				taxaIDsToInclude.put(taxon.getID(), true);
			}
			numTaxa = taxaIDsToInclude.size();
		}else {
			numTaxa = alignment.getNtaxa();
			for (int i = 0; i < numTaxa; i ++) taxaIDsToInclude.put(i, true);
		}
		
		
		this.majors = null;
		

		// Which sites to include?
		this.sitesToIncludeMap = new HashMap<>();
		this.sitesToIncludeList = new ArrayList<>();
		if (this.variantSitesOnly) {
			for (int site = 0; site < alignment.getLength(); site ++) {
				
				boolean includeSite = false;
				String uniqueSymbol = null;
				for (int taxa = 0; taxa < alignment.getNtaxa(); taxa ++) {
					
					Taxon taxon = alignment.getSequence(taxa).getTaxon();
					
					// Should this taxon be included?
					if (!this.includeTaxon(taxon)) continue;
					
					Sequence sequence = alignment.getSequence(taxa);
					
					// What is the symbol?
					String symbol = sequence.getSymbol(site);
					if (Alignment.isAmbiguousOrGap(symbol, sequence.isNucleotide())) continue;
					
					if (uniqueSymbol == null) {
						uniqueSymbol = symbol;
					}
					else if (!uniqueSymbol.equals(symbol)) {
						includeSite = true;
						break;
					}
					
					
					
				}
				
				
				// Include the site
				if (includeSite) {
					//System.out.println("including site " + site);
					this.sitesToIncludeMap.put(site, true);
					this.sitesToIncludeList.add(site);
				}
				
				
				
				
			}
			
			numSites = sitesToIncludeMap.size();
			if (numSites == 0) numSites = alignment.getLength();
		
		}else {
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
	 * Are variant sites only being displayed?
	 * @return
	 */
	public boolean variantSitesOnly() {
		return this.variantSitesOnly;
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
	
	
}















