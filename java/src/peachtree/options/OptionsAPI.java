package peachtree.options;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Set;

import org.json.JSONArray;
import org.json.JSONObject;
//import org.reflections.Reflections;

import peachtree.aln.AlignmentAPI;
import peachtree.aln.colourings.Aliview;
import peachtree.aln.colourings.ClustalAmino;
import peachtree.aln.colourings.Colouring;
import peachtree.aln.colourings.Drums;
import peachtree.aln.colourings.Jalview;
import peachtree.phy.ClusterTree;
import peachtree.phy.PhylogenyAPI;
import peachtree.phy.util.LinkType;



public class OptionsAPI {
	
	static final long CHUNK_SIZE = 30000;
	
	
	// Boundaries
	static NumericalOption canvasWidth  = new NumericalOption("width", "General", "Width of canvas", 1000, 100, 2000, 100, true);
	static NumericalOption canvasHeight  = new NumericalOption("height", "General", "Height of canvas", 500, 100, 2000, 100, true);
	static NumericalOption division1  = new NumericalOption("division1", "General", "Relative position of the tree/taxa boundary", 0.3, 0, 1, 0.1, true);
	static NumericalOption division2  = new NumericalOption("division2", "General", "Relative position of the taxa/alignment boundary", 0.5, 0, 1, 0.1, true);
	
	
	// Scroll bars
	static NumericalOption scrollY  = new NumericalOption("scrollY", "General", "Relative position of y-scrollbar", 0, 0, 1, 0.1, true);
	static NumericalOption scrollX  = new NumericalOption("scrollX", "General", "Relative position of x-scrollbar", 0, 0, 1, 0.1, true);
	
	
	static DiscreteOption treeMethods;
	static NumericalOption branchwidth = new NumericalOption("branchWidth", "Phylogeny", "Branch width", 2, 0.25, 20, 0.5);
	static NumericalOption treeSpacing = new NumericalOption("treeSpacing", "Phylogeny", "Horizontal padding around tree", 5, 0, 50, 5);
	
	
	static NumericalOption siteHeight = new NumericalOption("siteHeight", "Taxa", "Row heights", 20, 1, 100, 5);
	static NumericalOption fontSizeTaxa = new NumericalOption("fontSizeTaxa", "Taxa", "Font size of taxa", 16, 1, 50, 1);
	static NumericalOption taxaSpacing = new NumericalOption("taxaSpacing", "Taxa", "Padding before taxon names", 5, 0, 50, 5);
	
	
	static NumericalOption siteMinWidth = new NumericalOption("siteDim", "Alignment", "Minimum width of alignment sites", 15, 1, 100, 5);
	static NumericalOption fontSizeAln = new NumericalOption("fontSizeAln", "Alignment", "Font size of alignment", 16, 1, 50, 1);
	
	static DiscreteOption colourings;
	
	
	
	
		
	static List<Class<? extends Colouring>> colouringClasses;
	
	static JSONArray graphicalObjects = null;
	
	public static void init() throws Exception {
		
		graphicalObjects = null;
		
		
		
		treeMethods = new DiscreteOption("treeMethods", "Phylogeny", "Method for phylogenetic tree estimation", LinkType.neighborjoining, LinkType.values());
		
	}
	

	
	
	/**
	 * Prepares the colouring option so that only colour schemes applicable to the current alignment (aa or nt) are included
	 * @throws IllegalAccessException 
	 * @throws InstantiationException 
	 */
	public static void prepareColourings() throws Exception {
		
		
		// Find all colour classes. Reflections are not working in cheerpj so adding classes manually
		
		//Reflections reflections = new Reflections("peachtree.aln.colourings");
		//Set<Class<? extends Colouring>> classes = reflections.getSubTypesOf(Colouring.class);
		colouringClasses = new ArrayList<>();
		colouringClasses.add(ClustalAmino.class);
		colouringClasses.add(Jalview.class);
		colouringClasses.add(Aliview.class);
		colouringClasses.add(Drums.class);
		
		
		List<Colouring> colourObjects = new ArrayList<>();
		for (Class<? extends Colouring> colClass : colouringClasses) {
	        Colouring col = colClass.newInstance();
	        if (AlignmentAPI.colouringIsApplicable(col)) {
	        	 System.out.println(colClass.getName() + " is applicable");
	        	 colourObjects.add(col);
	        }
	       
		}
		
		
		colourings = new DiscreteOption("colourings", "Alignment", "Alignment colour scheme", colourObjects.get(0), colourObjects);
	}
	
	
	
	/**
	 * Set the value of this option
	 * @param id
	 * @param value
	 */
	public static String setOption(String id, String value) {
		
		
		try {
			
			// Find the option
			Option option = null;
			for (Option o : getOptionList()) {
				if (o.getName().equals(id)) {
					option = o;
					break;
				}
			}
			
			if (option == null) {
				System.out.println("Cannot find option " + id);
			}else {
				
				
				if (option instanceof NumericalOption) {
					((NumericalOption)option).setVal(Double.parseDouble(value));
				}
				
				if (option instanceof DiscreteOption) {
					((DiscreteOption)option).setVal(value);
				}
				
				
			}
				
		}catch(Exception e){
			e.printStackTrace();
			return getErrorJSON(e);
		}
		
		
		return "{}";
		
	}
	
	
	
	/**
	 * Build a tree from the alignment
	 * @return
	 */
	public static String buildTree() {
		
		try {
			
			LinkType method = LinkType.valueOf(((DiscreteOption)treeMethods).getVal().toString());
			System.out.println("building tree from " + method.toString());
			
			return PhylogenyAPI.buildTree(AlignmentAPI.getAlignment(), method);
		} catch (Exception e) {
			return getErrorJSON(e);
		}
		
	}
	
	
	/**
	 * Generate all objects. Now ready - to render onto the svg
	 * Return canvas width and height but do not return any of the objects until getGraphics is called
	 * @return
	 */
	public static String initGraphics() {
		
		
		if (!isReady()) return "{}";
		
		// Clear it so it stops rendering
		graphicalObjects = null;
		
		try {
		
			// Bounds
			double xdivide1 = division1.getVal();
			double xdivide2 = division2.getVal();
			
			JSONObject json = new JSONObject();
			double width = canvasWidth.getVal();
			double height = canvasHeight.getVal();
			
			
			// Scroll bars
			JSONObject scrolls = new JSONObject();
			
			
			// Height of taxa
			double ntHeight = siteHeight.getVal();
			if (AlignmentAPI.isReady()) {
				ntHeight = Math.max(Math.max(ntHeight,  fontSizeTaxa.getVal()), fontSizeAln.getVal());
			}
			System.out.println("ntHeight " + ntHeight);
			
			

			
			// x-boundary objects
			JSONObject xboundaries = new JSONObject();
			xboundaries.put(canvasWidth.getName(), width);
			xboundaries.put(division1.getName(), xdivide1*width);
			xboundaries.put(division2.getName(), xdivide2*width);
			json.put("xboundaries", xboundaries);
			
			// y-boundary objects
			JSONObject yboundaries = new JSONObject();
			yboundaries.put(canvasHeight.getName(), height);
			json.put("yboundaries", yboundaries);
			
			
			
			// Vertical scrolling?
			if (height < ntHeight * AlignmentAPI.getNtaxa()) {
				scrolls.put("scrollY", scrollY.getVal()*height);
			}
			height = ntHeight * AlignmentAPI.getNtaxa();
			
			
			
			json.put("scrolls", scrolls);
			
			
			
			// Create graphics
			JSONArray objs = new JSONArray();
			if (PhylogenyAPI.isReady()) {
				
				System.out.println("tree API");
				double spacing = treeSpacing.getVal();
				double branchW = branchwidth.getVal();
				
				JSONArray tree = PhylogenyAPI.getTreeGraphics(spacing, xdivide1*width - spacing, 0, height, branchW);
				objs.putAll(tree);
				
				
			}
			
			
			// Width of taxa
			if (AlignmentAPI.isReady()) {
				
			
			
				// Taxa
				double x0 = xdivide1*width + taxaSpacing.getVal();
				JSONArray taxa = AlignmentAPI.getTaxaGraphics(x0, xdivide2*width, 0, height,  fontSizeTaxa.getVal());
				objs.putAll(taxa);
				
	
				// Alignment
				double minWidth = siteMinWidth.getVal();
				Colouring cols = (Colouring) colourings.getVal();
				System.out.println("Using the " + cols.getName() + " scheme");
				JSONArray alignment = AlignmentAPI.getAlignmentGraphics(xdivide2*width, width, 0, height, minWidth, fontSizeAln.getVal(), cols);
				objs.putAll(alignment);
				
				json.put("nsites", AlignmentAPI.getNsites());
				json.put("nsitesdisplayed", AlignmentAPI.getNsitesDisplayed());
				json.put("ntaxa", AlignmentAPI.getNtaxa());
				
				
			}
			
			graphicalObjects = objs;
			
			System.out.println(json.toString());
			
			return json.toString();
		
		} catch (Exception e) {
			e.printStackTrace();
			return getErrorJSON(e);
		}
		
	}
	
	
	/**
	 * Return the json objects generated by initGraphics 1 chunk at a time
	 * @return
	 */
	public static String getGraphics() {
		
		if (graphicalObjects == null || graphicalObjects.length() == 0) return new JSONArray().toString();

		
		// Keep adding to the string until the string is too large
		StringBuilder out = new StringBuilder();
		out.append("[");
		int len = 0;
		boolean addedObj = false;
		do {
			String str = graphicalObjects.get(0).toString();
			if (str.length() + len < CHUNK_SIZE) {
				
				//System.out.println("Added json at position " + i + ". Total length " + );
				
				// Update string
				if (addedObj) out.append(",");
				addedObj = true;
				out.append(str);
				len = out.length();

				// Remove 1st element from from array
				graphicalObjects.remove(0);
				
			}else {
				
				// Chunk is too big. Move on.
				break;
			}
			
		}while(graphicalObjects.length() > 0);
		

		if (!addedObj) {
			System.out.println("Error: chunk sizes are too small");
		}

		out.append("]");
		
		return out.toString();
	}

	
	
	
	/**
	 * Get a list of options
	 * @return
	 * @throws Exception
	 */
	private static List<Option> getOptionList() throws Exception{
		
		
		
		
		// Introspectively find all options and add them to the list of options
		List<Option> options = new ArrayList<>();
		Field[] fields = OptionsAPI.class.getDeclaredFields();
		for (Field field: fields) {
			
			if (field.get(null) instanceof Option) {
					
				try {
					Option option = (Option) field.get(null);
					options.add(option);
				} catch (Exception e) {
					e.printStackTrace();
				} 
			}
		}
		
		return options;
		
	}
	
	
	/**
	 * Get the list of visual settings as a json string
	 * @return
	 */
	public static String getOptions() {
		
		try {
			
			
			JSONArray arr = new JSONArray();
			for (Option option : getOptionList()) {
				arr.put(option.toJSON());
			}
			return arr.toString();
			
		} catch(Exception e) {
			e.printStackTrace();
			return getErrorJSON(e);
		}
	}
	
	
	
	/**
	 * Gets a JSON string from an Exception
	 * @param e
	 * @return
	 */
	public static String getErrorJSON(Exception e) {
		JSONObject json = new JSONObject().put("err",  e.getMessage());  
		return json.toString();
	}
	
	
	/**
	 * Is the system ready to render?
	 * @return
	 */
	public static boolean isReady() {
		
		boolean ready = AlignmentAPI.isReady();
		return ready;
		
	}
	
	

}
