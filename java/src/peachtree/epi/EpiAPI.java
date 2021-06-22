package peachtree.epi;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import org.json.JSONObject;

import peachtree.aln.Alignment;
import peachtree.aln.AlignmentAPI;
import peachtree.options.OptionsAPI;
import peachtree.phy.PhylogenyAPI;
import peachtree.phy.Tree;

public class EpiAPI {
	
	
	private static Epidemiology EPIDEMIOLOGY = null;
	
	private static boolean epiAccessionsAreDirty = false;
	private static boolean epiAnnotationsAreDirty = false;
	
	
	/**
	 * Upload epidmiological information as a csv/tsv
	 * @param contents
	 * @return
	 */
	public static String uploadEpi(String contents) {
		
		System.out.println("Uploading epidemiological information");
		
		
		try {
			long start = Calendar.getInstance().getTimeInMillis();
			
			
			EPIDEMIOLOGY = new Epidemiology();
			EPIDEMIOLOGY.parseFile(contents);
			EpiAPI.setEpiAccessionsToDirty();
			EpiAPI.setEpiAnnotationsToDirty();
			EpiAPI.validateAccessions(AlignmentAPI.getAlignment());
			EpiAPI.addAnnotationsToTree(PhylogenyAPI.getTree());
			
			OptionsAPI.prepareEpiAnnotations();
			
			long finish = Calendar.getInstance().getTimeInMillis();
			JSONObject json = new JSONObject().put("time", (finish-start)/1000.0);
			return json.toString();
			
			
		}catch (Exception e){
			EPIDEMIOLOGY = null;
			e.printStackTrace();
			return OptionsAPI.getErrorJSON(e);
		}
		
		
	}
	
	
	/**
	 * Indicate that the accessions need to be revalidated
	 */
	public static void setEpiAccessionsToDirty() {
		epiAccessionsAreDirty = true;
	}
	
	
	/**
	 * Indicate that the annotations need to be added to the tree
	 */
	public static void setEpiAnnotationsToDirty() {
		epiAnnotationsAreDirty = true;
	}
	
	
	
	
	/**
	 * Validate the accessions in the epidemiology with those in the alignment
	 * @param alignment
	 * @throws Exception
	 */
	public static void validateAccessions(Alignment alignment) throws Exception {
		if (!epiAccessionsAreDirty) return;
		if (alignment == null) return;
		if (EPIDEMIOLOGY == null) return;
		EPIDEMIOLOGY.validateAccessions(alignment);
		epiAccessionsAreDirty = false;
	}
	
	
	
	
	/**
	 * Add annotations to the tree
	 * @param epi
	 * @throws Exception
	 */
	public static void addAnnotationsToTree(Tree tree) throws Exception {
		if (!epiAnnotationsAreDirty) return;
		if (tree == null) return;
		if (EPIDEMIOLOGY == null) return;
		EPIDEMIOLOGY.addAnnotationsToTree(tree);
		epiAnnotationsAreDirty = false;
	}
	


	public static List<String> getAllAnnotations() {
		if (EPIDEMIOLOGY == null) return new ArrayList<String>();
		return EPIDEMIOLOGY.getAnnotations();
	}

}
