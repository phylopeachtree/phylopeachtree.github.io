package peachtree.aln;



import java.util.Calendar;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.aln.colourings.Colouring;
import peachtree.options.OptionsAPI;
import peachtree.options.Scaling;

public class AlignmentAPI {

	
	private static Alignment THE_ALIGNMENT;
	private static Filtering filtering = null;
	
	public static void init() {
		THE_ALIGNMENT = null;
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
		
		try {
			long start = Calendar.getInstance().getTimeInMillis();
			THE_ALIGNMENT = new Alignment(str);
			initFiltering(true);
			OptionsAPI.prepareColourings();
			long finish = Calendar.getInstance().getTimeInMillis();
			System.out.println("Parsed successfully (" + (finish-start) + "ms)" );
			
			JSONObject json = new JSONObject().put("time", (finish-start));
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


	public static Alignment getAlignment() {
		return THE_ALIGNMENT;
	}
	
	
	
	/**
	 * Initialise filtering object, if not already initialised
	 * @param variantSitesOnly
	 */
	public static void initFiltering(boolean variantSitesOnly) {
		
		
		boolean initRequired = false;
		if (filtering == null) initRequired = true;
		else if (filtering.variantSitesOnly() != variantSitesOnly) initRequired = true;
		
		// Default filtering
		if (initRequired) {
			filtering = new Filtering(variantSitesOnly, null, THE_ALIGNMENT);
		}
		
		
	}

	/**
	 * Filtering object used for drawing alignment/taxa/tree
	 * @return
	 */
	public static Filtering getFiltering() {
		return filtering;
	}
	
	
}






