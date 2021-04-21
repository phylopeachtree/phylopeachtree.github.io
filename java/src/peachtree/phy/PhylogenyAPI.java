package peachtree.phy;


import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.aln.Alignment;
import peachtree.aln.AlignmentAPI;
import peachtree.aln.Filtering;
import peachtree.options.OptionsAPI;
import peachtree.options.Scaling;
import peachtree.phy.util.LinkType;

public class PhylogenyAPI {

	
	private static Tree THE_TREE;
	static boolean orderingIsDirty = false;
	
	
	public static String uploadTree(String contents) {
		
		System.out.println("Uploading tree " + contents.length());
		

		try {
			long start = Calendar.getInstance().getTimeInMillis();
			THE_TREE = new Tree();
			THE_TREE.parseFromNexus(contents);
			orderingIsDirty = true;
			
			// If alignment has been uploaded, check it matches the tree
			prepareLabelling(AlignmentAPI.getAlignment());
			
			// Prepare tree annotation options
			OptionsAPI.prepareTreeAnnotationOptions();
			OptionsAPI.resetScroll();
			
			long finish = Calendar.getInstance().getTimeInMillis();
			System.out.println("Parsed tree successfully (" + (finish-start) + "ms)" );
			
			JSONObject json = new JSONObject().put("time", (finish-start)/1000.0);
			return json.toString();
			
		}catch (Exception e){
			THE_TREE = null;
			e.printStackTrace();
			return OptionsAPI.getErrorJSON(e);
		}
		
		
		
		
	}
	
	
	/**
	 * Prepare the labelling between tree and alignment if necessary
	 * @param alignment
	 * @throws Exception
	 */
	public static void prepareLabelling(Alignment alignment) throws Exception {
		if (alignment == null) return;
		if (THE_TREE == null) return;
		if (!orderingIsDirty) return;
		sortTaxaByTree(THE_TREE, alignment);
		AlignmentAPI.setOrderingToDirty();
		orderingIsDirty = false;
	}
	
	
	
	/**
	 * Build a tree from the alignment using the specified method
	 * @param alignment
	 * @param method
	 * @return
	 */
	public static String buildTree(Alignment alignment, LinkType method) {
		
		
		//String str = new String(contents);
		System.out.println("Creating tree " + method.toString());
		
		
		try {
			long start = Calendar.getInstance().getTimeInMillis();
			
			
			// Build tree
			THE_TREE = new ClusterTree(alignment, method);
			
			
			// Sort taxa by tree
			sortTaxaByTree(THE_TREE, alignment);
			AlignmentAPI.setOrderingToDirty();
			orderingIsDirty = false;
			
			
			// Prepare tree annotation options
			OptionsAPI.prepareTreeAnnotationOptions();
			OptionsAPI.resetScroll();
			
			long finish = Calendar.getInstance().getTimeInMillis();
			System.out.println("Built successfully (" + (finish-start) + "ms)" );
			
			JSONObject json = new JSONObject().put("time", (finish-start)).put("newick", THE_TREE.toNewick());
			return json.toString();
			
		}catch (Exception e){
			THE_TREE = null;
			e.printStackTrace();
			return OptionsAPI.getErrorJSON(e);
		}
		
		
	}
	
	
	/**
	 * Sort the taxa in the alignment by the tree
	 * @param alignment
	 */
	private static void sortTaxaByTree(Tree tree, Alignment alignment) throws Exception {
		alignment.sortByTree(tree);
	}
	
	
	
	/**
	 * Apply the filtering to this tree
	 */
	public static void applyFiltering(Filtering filtering) {
		if (THE_TREE == null) return;
		THE_TREE.applyFiltering(filtering);
	}
	
	/**
	 * Generate graphics for the tree
	 * @param scaling
	 * @param branchWidth
	 * @return
	 */
	public static JSONArray getTreeGraphics(Scaling scaling, double branchWidth, boolean showTaxaOnTree, double nodeRadius, String internalLabel, String leafLabel, double fontSize, int rounding) {
		return THE_TREE.getTreeGraphics(scaling, branchWidth, AlignmentAPI.getFiltering(), showTaxaOnTree, nodeRadius, internalLabel, leafLabel, fontSize, rounding);
	}
	
	
	
	/**
	 * Is the tree ready to go?
	 * @return
	 */
	public static boolean isReady() {
		return THE_TREE != null;
	}


	/**
	 * Height of current tree
	 * @return
	 */
	public static double getHeight() {
		return THE_TREE.getHeight();
	}


	public static Tree getTree() {
		return THE_TREE;
	}


	/**
	 * Return a list of annotations
	 * @return
	 */
	public static List<String> getAllAnnotations() {
		if (THE_TREE == null) return new ArrayList<>();
		return THE_TREE.getAllAnnotations();
	}
	
	
	
	
	
}
