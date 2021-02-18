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
	Map<Integer, Boolean> sitesToInclude;
	int numSites;
	
	public Filtering(boolean variantSitesOnly, List<Taxon> taxaToInclude, Alignment alignment) {
		this.variantSitesOnly = variantSitesOnly;
		
		
		// Use unique taxa ids
		this.taxaIDsToInclude = new HashMap<>();
		if (taxaToInclude != null) {
			for (Taxon taxon: taxaToInclude) {
				taxaIDsToInclude.put(taxon.getID(), true);
			}
		}
		
		

		// Which sites to include?
		this.sitesToInclude = new HashMap<>();
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
					
					if (uniqueSymbol == null) {
						if (!sequence.isAmbiguousCharOrGap(symbol)) uniqueSymbol = symbol;
					}
					else if (!uniqueSymbol.equals(symbol)) {
						includeSite = true;
						break;
					}
					
					
					
				}
				
				
				// Include the site
				if (includeSite) {
					//System.out.println("including site " + site);
					this.sitesToInclude.put(site, true);
				}
				
				
				
				
			}
			
			numSites = sitesToInclude.size();
			if (numSites == 0) numSites = alignment.getLength();
		
		}else {
			numSites = alignment.getLength();
		}
		
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
	 * Should this site number be included
	 * @param site
	 * @return
	 */
	public boolean includeSite(int site) {
		if (!this.variantSitesOnly) return true;
		if (this.sitesToInclude == null) return true;
		if (this.sitesToInclude.isEmpty()) return true;
		return this.sitesToInclude.containsKey(site);
	}
	
	
	public int[] getSites() {
		int[] sites = new int[this.getNumSites()];
		if (this.sitesToInclude != null && this.sitesToInclude.size() > 0) {
			int i = 0;
			for (int site : this.sitesToInclude.keySet()) {
				//System.out.println("including site " + site);
				sites[i] = site;
				i++;
			}
		}else {
			for (int i = 0; i < this.getNumSites(); i ++) {
				sites[i] = i;
				i++;
			}
		}
		return sites;
	}


	/**
	 * Number of sites included by filter
	 * @return
	 */
	public int getNumSites() {
		return numSites;
	}
	
	
	
	
}
