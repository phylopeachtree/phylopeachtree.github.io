package peachtree.options;

import java.lang.reflect.Field;
import java.math.BigDecimal;
import java.math.MathContext;
import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONObject;
//import org.reflections.Reflections;

import peachtree.aln.AlignmentAPI;
import peachtree.aln.Taxon;
import peachtree.aln.colourings.Aliview;
import peachtree.aln.colourings.ClustalAmino;
import peachtree.aln.colourings.Colouring;
import peachtree.aln.colourings.Drums;
import peachtree.aln.colourings.Jalview;
import peachtree.aln.colourings.SiteColourFilter;
import peachtree.phy.PhylogenyAPI;
import peachtree.phy.util.LinkType;



public class OptionsAPI {
	
	static final long CHUNK_SIZE = 30000;
	static final double LEFT_MARGIN = 16; // Left hand margin
	static final double TOP_MARGIN = 16; // Top margin
	
	static final double INIT_WIDTH = 1200;
	static final double INIT_HEIGHT = INIT_WIDTH*0.618;
	static final double INIT_DIV1 = 0.3;
	static final double INIT_DIV2 = 0.5;
	
	static final double SCROLL_Y_NROWS = 10; 
	
	// Boundaries
	static NumericalOption canvasWidth  = new NumericalOption("width", "General", "Width of canvas", INIT_WIDTH, 10, 2000, 100, true);
	static NumericalOption canvasHeight  = new NumericalOption("height", "General", "Height of canvas", INIT_HEIGHT, 10, 2000, 100, true);
	static NumericalOption division1  = new NumericalOption("division1", "General", "Relative position of the tree/taxa boundary", 0, 0, 1, 0.1, true);
	static NumericalOption division2  = new NumericalOption("division2", "General", "Relative position of the taxa/alignment boundary", INIT_DIV2, 0, 1, 0.1, true);
	
	
	// Scroll bars
	static NumericalOption scrollY  = new NumericalOption("scrollY", "General", "Relative position of y-scrollbar", 0, 0, 1, 0.1, true);
	static NumericalOption scrollX  = new NumericalOption("scrollX", "General", "Relative position of x-scrollbar", 0, 0, 1, 0.1, true);
	
	
	// Phylogeny
	static DiscreteOption treeMethods;
	static NumericalOption branchwidth = new NumericalOption("branchWidth", "Phylogeny", "Branch width", 2, 0.25, 20, 0.5);
	static NumericalOption nodeRadius = new NumericalOption("nodeRadius", "Phylogeny", "Node radius", 3, 0, 20, 0.5);
	static NumericalOption treeSpacing = new NumericalOption("treeSpacing", "Phylogeny", "Horizontal padding around tree", 5, 0, 50, 5);
	static BooleanOption showTaxaOnTree = new BooleanOption("showTaxaOnTree", "Phylogeny", "Indicate taxa on tree", true);
	static BooleanOption transmissionTree = new BooleanOption("transmissionTree", "Phylogeny", "Display as a transmission tree", false);
	static DiscreteOption internalNodeLabels;
	static DiscreteOption leafNodeLabels;
	static NumericalOption annotationFontSize = new NumericalOption("annotationFontSize", "Phylogeny", "Tree annotation font size", 8, 0, 14, 1, true);
	static NumericalOption annotationRounding = new NumericalOption("annotationRounding", "Phylogeny", "Tree annotation sf", 3, 1, 8, 1, true);
	
	// Taxa
	static NumericalOption siteHeight = new NumericalOption("siteHeight", "Samples", "Row heights", 20, 1, 100, 5);
	static NumericalOption fontSizeTaxa = new NumericalOption("fontSizeTaxa", "Samples", "Font size of samples", 13, 0, 50, 1);
	static NumericalOption taxaSpacing = new NumericalOption("taxaSpacing", "Samples", "Padding before sample names", 5, 0, 50, 1);
	static BooleanOption showTaxonNumbers = new BooleanOption("showTaxonNumbers", "Samples", "Show sample numbers", false);
	static BooleanOption focusOnTaxa = new BooleanOption("focusOnTaxa", "Samples", "Show only selected samples", false, true);
	static BooleanOption focusOnClade = new BooleanOption("focusOnClade", "Samples", "Show only clade of selected samples", false, true);
	
	// Alignment
	static NumericalOption ntWidth = new NumericalOption("ntWidth", "Alignment", "Width of alignment sites", 15, 1, 100, 5);
	static NumericalOption fontSizeAln = new NumericalOption("fontSizeAln", "Alignment", "Font size of alignment", 16, 0, 50, 1);
	static BooleanOption variantSitesOnly = new BooleanOption("variantSitesOnly", "Alignment", "Show segregating sites only", true);
	static DiscreteOption siteColourType;
	static DiscreteOption colourings;
	

		
	static List<Class<? extends Colouring>> colouringClasses;
	
	static JSONArray graphicalObjects = null;
	
	
	// Taxon to scroll to next time
	static Taxon focalTaxon = null;
	
	public static void init() throws Exception {
		
		graphicalObjects = null;
		focalTaxon = null;
		internalNodeLabels = null;
		leafNodeLabels = null;
		
		treeMethods = new DiscreteOption("treeMethods", "Phylogeny", "Method for phylogenetic tree estimation", LinkType.neighborjoining, LinkType.values());
		siteColourType = new DiscreteOption("siteColourType", "Alignment", "Which sites should be coloured", SiteColourFilter.all, SiteColourFilter.values());
		
	}
	
	
	
	/**
	 * Reset the scrollbars to 0,0
	 */
	public static void resetScroll() {
		scrollX.setVal(0);
		scrollY.setVal(0);
	}
	
	
	/**
	 * Scroll up/down slightly
	 * @param goingUp
	 */
	public static void scrollABit(int dir) {
		double y1 = scrollY.getVal();
		double relativeScrollAmnt = SCROLL_Y_NROWS/AlignmentAPI.getNtaxaDisplayed();
		double y2 = y1 + dir*relativeScrollAmnt;
		scrollY.setVal(y2);
		System.out.println("scrolling " + y1 + " -> " + y2 + " / " + scrollY.getVal());
	}
	
	
	
	/**
	 * Reset window width/height
	 */
	public static void resetWindowSize() {
		canvasWidth.setVal(INIT_WIDTH);
		canvasHeight.setVal(INIT_HEIGHT);
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
	 * Prepares all node annotations
	 * @throws Exception
	 */
	public static void prepareTreeAnnotationOptions() throws Exception {
		
		
		if (INIT_DIV1 > division2.getVal()) {
			division2.setVal(INIT_DIV2);
		}
		division1.setVal(INIT_DIV1);
		
		List<String> annotations = PhylogenyAPI.getAllAnnotations();
		if (annotations.isEmpty()) {
			annotationFontSize.hide();
			annotationRounding.hide();
			internalNodeLabels = null;
			leafNodeLabels = null;
			return;
		}
		annotations.add(0, "None");
		annotationFontSize.show();
		annotationRounding.show();
		internalNodeLabels = new DiscreteOption("internalNodeLabels", "Phylogeny", "Internal node labels", annotations.get(0), annotations);
		leafNodeLabels = new DiscreteOption("leafNodeLabels", "Phylogeny", "Leaf labels", annotations.get(0), annotations);
		
		
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
					
					double val = Double.parseDouble(value);
					
					// Special case: scroll bar positions should be normalised
					if (option == scrollX) {
						val = val - (canvasWidth.getVal())*division2.getVal();
						val = val / (canvasWidth.getVal() - (canvasWidth.getVal())*division2.getVal());
					}
					
					if (option == scrollY) {
						val = val / (canvasHeight.getVal());
					}
					
					
					((NumericalOption)option).setVal(val);
				}
				
				if (option instanceof BooleanOption) {
					
					boolean val = Boolean.parseBoolean(value);

					// Special case: if focusOnClade is enabled, then enable focusOnTaxa too
					if (option == focusOnClade && val == true) focusOnTaxa.setVal(true);
					
					// If focusOnTaxa is enabled, then set focusOnClade to false
					if (option == focusOnTaxa && val == true) focusOnClade.setVal(false);
					if (option == focusOnClade || option == focusOnTaxa) {
						resetScroll();
						AlignmentAPI.setSelectionToDirty();
					}
					
					((BooleanOption)option).setVal(val);
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
			
			
			// Initialise filterings if necessary
			AlignmentAPI.initFiltering(variantSitesOnly.getVal(), focusOnTaxa.getVal(), (focusOnClade.getVal() ? PhylogenyAPI.getTree() : null));
			
			// Prepare tree-alignment labellings if necessary
			PhylogenyAPI.prepareLabelling(AlignmentAPI.getAlignment());
			
			
			// Scroll bars
			JSONObject scrolls = new JSONObject();
			
			
			// Height of taxa
			double ntHeight = siteHeight.getVal();
			System.out.println("ntHeight " + ntHeight);
			
			
			// Full size of view
			double fullHeight = ntHeight * AlignmentAPI.getNtaxaDisplayed() + TOP_MARGIN;
			double fullAlnWidth = ntWidth.getVal() * (AlignmentAPI.getNsitesDisplayed()+1);
			
			
			
			// Vertical scrolling?
			if (height < fullHeight) {
				Scaling scaling = new Scaling(0, 0, TOP_MARGIN, height);
				scaling.setRowHeight(ntHeight);
				scaling.setScroll(0, scrollY.getVal(), 0, fullHeight);
				
				
				// Scroll to a taxon?
				if (focalTaxon != null) {
					
					
					
					// What row number?
					int rowNum = AlignmentAPI.getTaxonRowNum(focalTaxon);
					
					// Scroll Y value
					scaling.setScroll(0, 0, 0, fullHeight);
					double ypos = scaling.scaleY(rowNum) - TOP_MARGIN - height / 2;
					scrollY.setVal(ypos / (fullHeight-TOP_MARGIN));
					scaling.setScroll(0, scrollY.getVal(), 0, fullHeight);
					
					System.out.println("Setting scrolly to " + (ypos / (fullHeight-TOP_MARGIN)) + " to see " + focalTaxon.getName());
					
				}
				
				
				// Validate scrollY position
				if (scrollY.getVal()*height + scaling.getScrollYLength() > height) {
					scrollY.setVal((height - scaling.getScrollYLength()) / height);
				}
				scrolls.put("scrollY", scrollY.getVal()*height).put("scrollYLength", scaling.getScrollYLength());
			}else {
				canvasHeight.setVal(fullHeight);
				height = canvasHeight.getVal();
			}
			
			// Reset focal taxon
			focalTaxon = null;
			
			
			// Shrink width?
			if (width*(1-xdivide2) > fullAlnWidth) {
				canvasWidth.setVal(width*xdivide2 + fullAlnWidth);
				width = canvasWidth.getVal();
				division2.setVal((width-fullAlnWidth)/width);
				xdivide2 = division2.getVal();
			}
			

			
			// Ensure divs are within margins
			if (xdivide1*width < LEFT_MARGIN) {
				xdivide1 = LEFT_MARGIN / width;
			}
			if (xdivide2*width < LEFT_MARGIN) {
				xdivide2 = LEFT_MARGIN / width;
			}
			
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
			

			
			
			
			// Create graphics
			JSONArray objs = new JSONArray();
			if (PhylogenyAPI.isReady()) {
				
				
				PhylogenyAPI.applyFiltering(AlignmentAPI.getFiltering());
				
				
				double branchW = branchwidth.getVal();
				double nodeRad = nodeRadius.getVal();
				double spacing = Math.max(treeSpacing.getVal(), nodeRad);
				
				
				// How tall is the tree?
				double treeHeight = PhylogenyAPI.getHeight();
				double treeMin = 0;
				if (AlignmentAPI.getFiltering().getSubtreeRoot() != null) {
					treeHeight = AlignmentAPI.getFiltering().getSubtreeRoot().getHeight();
					treeMin = AlignmentAPI.getFiltering().getSubtreeRoot().getYoungestChildHeight();
				}
				
				
				// Scaling
				Scaling scaling = new Scaling(LEFT_MARGIN + spacing,  xdivide1*width - spacing, TOP_MARGIN, height, treeHeight, treeMin);
				scaling.setRowHeight(ntHeight);
				scaling.setScroll(0, scrollY.getVal(), 0, fullHeight);
				
				// Annotations
				String internalLabel = internalNodeLabels == null ? null : internalNodeLabels.getVal().toString();
				String leafLabels = leafNodeLabels == null ? null : leafNodeLabels.getVal().toString();
				double fontSize = Math.min(ntHeight, annotationFontSize.getVal());
				int rounding = (int)annotationRounding.getVal();
				
				JSONArray tree = PhylogenyAPI.getTreeGraphics(scaling, branchW, showTaxaOnTree.getVal(), nodeRad, internalLabel, leafLabels, fontSize, rounding, transmissionTree.getVal());
				objs.putAll(tree);
				
				
			}
			
			
			// Width of taxa
			if (AlignmentAPI.isReady()) {

			
				// Taxa
				if (xdivide2 > xdivide1) {
					double x0 = xdivide1*width + taxaSpacing.getVal();
					
					// Scaling
					Scaling scaling = new Scaling(x0, xdivide2*width, TOP_MARGIN, height);
					scaling.setRowHeight(ntHeight);
					scaling.setScroll(0, scrollY.getVal(), 0, fullHeight);
					
					// Font size
					double labelFontSize = Math.min(ntHeight, fontSizeTaxa.getVal());
					
					JSONArray taxa = AlignmentAPI.getTaxaGraphics(scaling,  labelFontSize, showTaxonNumbers.getVal());
					objs.putAll(taxa);
				}
				
	
				// Alignment
				double alnViewWidth = (width - xdivide2*width);
				
				if (alnViewWidth > 0) {
					
					double minWidth = ntWidth.getVal();
					int nsitesInView = (int) Math.ceil(alnViewWidth /  minWidth); // *AlignmentAPI.getNsitesDisplayed();
					Colouring cols = (Colouring) colourings.getVal();
					cols.setSiteColourFilter((SiteColourFilter)siteColourType.getVal(), AlignmentAPI.getFiltering());
					System.out.println("Using the " + cols.getName() + " scheme");
					
					// Scaling
					Scaling scaling = new Scaling(xdivide2*width, width, TOP_MARGIN, height, 0, nsitesInView-1);
					scaling.setRowHeight(ntHeight);
					scaling.setScroll(scrollX.getVal(), scrollY.getVal(), fullAlnWidth, fullHeight);
					
					
					// Font size
					double ntFontSize = Math.min(ntHeight, fontSizeAln.getVal());
					
					JSONArray alignment = AlignmentAPI.getAlignmentGraphics(scaling, minWidth, ntFontSize, cols);
					objs.putAll(alignment);
					
					json.put("nsites", AlignmentAPI.getNsites());
					json.put("nsitesdisplayed", AlignmentAPI.getNsitesDisplayed());
					json.put("ntaxa", AlignmentAPI.getNtaxa());
					json.put("ntaxadisplayed", AlignmentAPI.getNtaxaDisplayed());
					json.put("nuniqueseqs", AlignmentAPI.getNumUniqueSequences());
					
					
					// Horizontal scrolling?
					if (scaling.getScrollXLength() > 0) {
						
						// Validate scrollX position
						if (scrollX.getVal()*alnViewWidth + scaling.getScrollXLength() > alnViewWidth) {
							scrollX.setVal((alnViewWidth - scaling.getScrollXLength()) / alnViewWidth);
						}
						
						scrolls.put("scrollX", scrollX.getVal()* (width - xdivide2*width) + xdivide2*width).put("scrollXLength", scaling.getScrollXLength());
					}
					
				
				}
				
				
				
			}
			
			
			
			// Add top layer objects
			JSONArray topObjs = new JSONArray();
			
			// Top and left margin backgrounds
			JSONObject top  = new JSONObject().put("ele", "rect").put("x", 0).put("y", 0).put("width", width).put("height", TOP_MARGIN).put("fill", "#FFDAB9");
			JSONObject left = new JSONObject().put("ele", "rect").put("x", 0).put("y", 0).put("width", LEFT_MARGIN).put("height", height).put("fill", "#FFDAB9");
			topObjs.put(top).put(left);
			json.put("objects", topObjs);
			
			
			graphicalObjects = objs;
			
			
			json.put("scrolls", scrolls);
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
					if (option != null) options.add(option);
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
	 * Declare the current taxon label as the focal label so it can be zoomed in on and selected
	 */
	public static void searchForTaxon(String label) {
		
		focalTaxon = null;
		
		
		// Get the taxon
		Taxon taxon = AlignmentAPI.getTaxon(label);
		if (taxon == null) {
			System.out.println("Warning: cannot find " + label);
			return;
		}
		
		//System.out.println("Found " + taxon.getName());
		
		
		// Select it
		taxon.isSelected(true);
		
		
		// Set it as the focal taxon
		focalTaxon = taxon;

		
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
		boolean ready = AlignmentAPI.isReady(); // || PhylogenyAPI.isReady();
		return ready;
	}



	/**
	 * Are variant sites only being displayed
	 * @return
	 */
	public static boolean variantSitesOnly() {
		return variantSitesOnly.getVal();
	}
	
	
	
	/**
	 * Are selected taxa being focused on
	 * @return
	 */
	public static boolean focusingOnTaxa() {
		return focusOnTaxa.getVal();
	}
	
	
	/**
	 * Set whether taxa are being focused on
	 * @return
	 */
	public static void focusingOnTaxa(boolean val) {
		focusOnTaxa.setVal(val);
	}
	
	/**
	 * Is a clade being focused on
	 * @return
	 */
	public static boolean focusOnClade() {
		return focusOnClade.getVal();
	}
	
	/**
	 * Set whether a clade is being focused on
	 * @return
	 */
	public static void focusOnClade(boolean val) {
		focusOnClade.setVal(val);
	}
	
	
	
    
    /**
     * Round to to 4sf
     * @param d
     * @return
     */
    public static double sf(double d) {
    	BigDecimal bd = new BigDecimal(d);
    	bd = bd.round(new MathContext(4));
    	return bd.doubleValue();
    }
	

}
