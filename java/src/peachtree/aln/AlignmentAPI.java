package peachtree.aln;



import java.util.Calendar;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.aln.colourings.Colouring;
import peachtree.epi.EpiAPI;
import peachtree.options.OptionsAPI;
import peachtree.options.Scaling;
import peachtree.phy.PhylogenyAPI;
import peachtree.phy.Tree;

public class AlignmentAPI {

	
	private static Alignment THE_ALIGNMENT;
	private static Filtering filtering = null;
	private static boolean selectionIsDirty = false;
	private static boolean orderingIsDirty = false;
	private static int mostRecentlySelectedTaxon = -1;
	
	public static void init() {
		THE_ALIGNMENT = null;
	}
	
	
	/**
	 * Notify the API that the selection is dirty and therefore the filtering needs to be constructed again
	 */
	public static void setSelectionToDirty() {
		selectionIsDirty = true;
	}
	
	/**
	 * Notify the API that the taxon ordering has changed and therefore the filtering needs to be constructed again
	 */
	public static void setOrderingToDirty() {
		orderingIsDirty = true;
	}
	
	
	
	/**
	 * Return a list of taxa labels, as a json string
	 * @return
	 */
	public static String getListOfTaxaLabels() {
		
		
		
		try {
			
			JSONObject json = new JSONObject();
			JSONArray labels = new JSONArray();
			if (THE_ALIGNMENT != null) {
			
				// Find all displayed taxa
				for (Sequence seq : THE_ALIGNMENT.getSequences()) {
					Taxon taxon = seq.getTaxon();
					if (filtering.includeTaxon(taxon)) {
						labels.put(taxon.getName());
					}
				}
			
			}
			
			json.put("labels", labels);
			return json.toString();
			
		}catch (Exception e){
			e.printStackTrace();
			return OptionsAPI.getErrorJSON(e);
		}
	}
	
	

	
	/**
	 * Set the global alignment used in this session
	 * @param str
	 * @return
	 * @throws Exception
	 */
	public static String uploadAlignment(String str) {
		
		
		
		//String str = new String(contents);
		System.out.println("Uploading alignment " + str.length());
		filtering = null;
		selectionIsDirty = false;
		mostRecentlySelectedTaxon = -1;
		
		try {
			long start = Calendar.getInstance().getTimeInMillis();
			THE_ALIGNMENT = new Alignment(str);
			long t0 = Calendar.getInstance().getTimeInMillis();
			initFiltering(OptionsAPI.variantSitesOnly(), false, null);
			OptionsAPI.prepareColourings();
			long finish = Calendar.getInstance().getTimeInMillis();
			
			// If tree has been uploaded, check the alignment matches the tree
			PhylogenyAPI.prepareLabelling(THE_ALIGNMENT);
			EpiAPI.setEpiAccessionsToDirty();
			EpiAPI.validateAccessions(THE_ALIGNMENT);
			OptionsAPI.resetScroll();
			OptionsAPI.resetWindowSize();
			
			
			System.out.println((finish-t0) + "ms on filtering" );
			System.out.println("Parsed successfully (" + (finish-start) + "ms)" );
			
			JSONObject json = new JSONObject().put("time", (finish-start)/1000.0);
			return json.toString();
			
		}catch (Exception e){
			THE_ALIGNMENT = null;
			e.printStackTrace();
			return OptionsAPI.getErrorJSON(e);
		}
		
		
		
		// 1613615862759->1613615862759
		
		
		 //THE_ALIGNMENT.toJSON().toString();
		
	}
	
	
	/**
	 * Is the alignment ready to go?
	 * @return
	 */
	public static boolean isReady() {
		return THE_ALIGNMENT != null;
	}
	
	
	
	/**
	 * Select the taxon
	 * @param taxonNum
	 */
	public static void selectTaxon(int taxonNum) {
		mostRecentlySelectedTaxon = taxonNum;
		THE_ALIGNMENT.selectTaxon(taxonNum);
		setSelectionToDirty();
		
	}
	
	/**
	 * Select/deselect everything between the most recently selected taxon and this one
	 */
	public static String selectUpToTaxon(int newTaxonNum) {
		
		
		JSONArray toColour = new JSONArray();
		
		
		// Toggle the currently selected one
		if (newTaxonNum == mostRecentlySelectedTaxon) {
			boolean isSelected = THE_ALIGNMENT.selectTaxon(newTaxonNum);
			JSONObject obj = new JSONObject().put("i", newTaxonNum).put("select", isSelected);
			toColour.put(obj);
		}
		else {
			
			// Set all the selected ones to the same state as the previously selected one
			boolean setTo = THE_ALIGNMENT.taxonIsSelected(mostRecentlySelectedTaxon);
			
			if (newTaxonNum > mostRecentlySelectedTaxon) {
				for (int i = mostRecentlySelectedTaxon+1; i <= newTaxonNum; i ++ ) {
					
					Taxon taxon = THE_ALIGNMENT.getTaxon(i);
					if (filtering != null && !filtering.includeTaxon(taxon)) continue;
					
					THE_ALIGNMENT.selectTaxon(i, setTo);
					JSONObject obj = new JSONObject().put("i", i).put("select", setTo);
					toColour.put(obj);
				}
			}else {
				for (int i = mostRecentlySelectedTaxon-1; i >= newTaxonNum; i -- ) {
					
					Taxon taxon = THE_ALIGNMENT.getTaxon(i);
					if (filtering != null && !filtering.includeTaxon(taxon)) continue;
					
					THE_ALIGNMENT.selectTaxon(i, setTo);
					JSONObject obj = new JSONObject().put("i", i).put("select", setTo);
					toColour.put(obj);
				}
			}
		}
		
		setSelectionToDirty();
		
		return toColour.toString();
		
	}
	
	/**
	 * Deselect all taxa
	 */
	public static boolean clearSelection() {
		THE_ALIGNMENT.clearSelection();
		mostRecentlySelectedTaxon=-1;
		OptionsAPI.focusingOnTaxa(false);
		OptionsAPI.focusOnClade(false);
		OptionsAPI.resetWindowSize();
		setSelectionToDirty();
		return filtering.focusing();
	}
	
	


	
	/**
	 * Get the graphics for the alignment within the bounds
	 * @param xmin
	 * @return
	 */
	public static JSONArray getAlignmentGraphics(Scaling scaling, double minNtWidth, double textSize, Colouring colouring) {
		return THE_ALIGNMENT.getAlignmentGraphics(scaling, colouring, minNtWidth, textSize, filtering);
	}
	
	
	
	/**
	 * Get the graphics for the taxa
	 * @param xmin
	 * @param xmax
	 * @param ymin
	 * @param ymax
	 * @return
	 */
	public static JSONArray getTaxaGraphics(Scaling scaling, double textSize, boolean showTaxonNumbers) {
		return THE_ALIGNMENT.getTaxaGraphics(scaling, textSize, filtering, showTaxonNumbers);
	}


	/**
	 * Is this colouring scheme applicable to the current alignment?
	 */
	public static boolean colouringIsApplicable(Colouring colouring) {
		if (THE_ALIGNMENT == null) return true;
		return THE_ALIGNMENT.isNucleotide() == colouring.isNucleotide();
	}


	public static int getNsites() {
		if (THE_ALIGNMENT == null) return 0;
		return THE_ALIGNMENT.getLength();
	}
	
	public static int getNsitesDisplayed() {
		if (THE_ALIGNMENT == null) return 0;
		if (filtering == null) return 0;
		return filtering.getNumSites();
	}
	
	public static int getNtaxa() {
		if (THE_ALIGNMENT == null) return 0;
		return THE_ALIGNMENT.getNtaxa();
	}
	
	public static int getNtaxaDisplayed() {
		if (THE_ALIGNMENT == null) return 0;
		if (filtering == null) return 0;
		return filtering.getNumSeqs();
	}


	public static Alignment getAlignment() {
		return THE_ALIGNMENT;
	}
	
	
	
	/**
	 * Initialise filtering object, if not already initialised
	 * @param variantSitesOnly
	 */
	public static void initFiltering(boolean variantSitesOnly, boolean focus, Tree tree) {
		
		
		if (THE_ALIGNMENT == null) return;
		
		boolean initRequired = false;
		if (filtering == null) initRequired = true;
		else if (filtering.focusing() && selectionIsDirty) initRequired = true;
		else if (filtering.variantSitesOnlyParsed() != variantSitesOnly) initRequired = true;
		else if (filtering.focusing() != focus) initRequired = true;
		else if (filtering.getTree() != tree) initRequired = true;
		else if (orderingIsDirty) initRequired = true;
		
		
		// Default filtering
		if (initRequired) {
			


			
			
			filtering = new Filtering(variantSitesOnly, focus, THE_ALIGNMENT, tree);
			selectionIsDirty = false;
			orderingIsDirty = false;
		}
		
		
	}

	/**
	 * Filtering object used for drawing alignment/taxa/tree
	 * @return
	 */
	public static Filtering getFiltering() {
		return filtering;
	}

	
	/**
	 * Get the taxon object
	 * @param label
	 * @return
	 */
	public static Taxon getTaxon(String label) {
		return THE_ALIGNMENT.getTaxon(label);
	}


	
	/**
	 * Find the row number of this taxon in the filtering
	 * @param taxon
	 * @return
	 */
	public static int getTaxonRowNum(Taxon taxon) {
		return filtering.getIndex(taxon);
	}
	

	/**
	 * The number of unique sequences in the alignment
	 * @return
	 */
	public static int getNumUniqueSequences() {
		if (filtering == null) return 0;
		return filtering.getNumUniqueSequences();
	}
	
}






