package peachtree.options;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import org.json.JSONArray;
import org.json.JSONObject;
//import org.reflections.Reflections;

import peachtree.aln.AlignmentAPI;
import peachtree.aln.colourings.ClustalAminoColouring;
import peachtree.aln.colourings.Colouring;
import peachtree.aln.colourings.JalviewNucleotideColouring;



public class OptionsAPI {
	

	
	static Option canvasWidth  = new NumericalOption("width", "General", "Width of canvas", 1000, 100, 2000);
	static Option canvasHeight  = new NumericalOption("height", "General", "Height of canvas", 700, 100, 2000);
	static Option division1  = new NumericalOption("division1", "General", "Relative position of the tree/taxa boundary", 0.1, 0, 1);
	static Option division2  = new NumericalOption("division2", "General", "Relative position of the taxa/alignment boundary", 0.3, 0, 1);
	
	static Option siteDim = new NumericalOption("siteDim", "Alignment", "Width and height of an aligned site", 20, 1, 100);
	static Option taxaSpacing = new NumericalOption("taxaSpacing", "Taxa", "Padding before taxon names", 5, 0, 50);
	static Option colourings;
	
	static Option branchwidth = new NumericalOption("branchWidth", "Phylogeny", "Branch width", 20, 1, 100);
	
		
	static List<Class<? extends Colouring>> colouringClasses;
	static final long CHUNK_SIZE = 30000;
	static JSONArray graphicalObjects = null;
	
	public static void init() throws Exception {
		
		graphicalObjects = null;
		
		
	}
	

	
	
	/**
	 * Prepares the colouring option so that only colour schemes applicable to the current alignment (aa or nt) are included
	 * @throws IllegalAccessException 
	 * @throws InstantiationException 
	 */
	private static void prepareColourings() throws Exception {
		
		
		// Find all colour classes. Reflections are not working in cheerpj so adding classes manually
		
		//Reflections reflections = new Reflections("peachtree.aln.colourings");
		//Set<Class<? extends Colouring>> classes = reflections.getSubTypesOf(Colouring.class);
		colouringClasses = new ArrayList<>();
		colouringClasses.add(ClustalAminoColouring.class);
		colouringClasses.add(JalviewNucleotideColouring.class);
		
		
		List<Colouring> classes = new ArrayList<>();
		for (Class<? extends Colouring> colClass : colouringClasses) {
	        Colouring col = colClass.newInstance();
	        if (AlignmentAPI.colouringIsApplicable(col)) {
	        	 System.out.println(colClass.getName() + " is applicable");
	        	 classes.add(col);
	        }
	       
		}
		colourings = new DiscreteOption("colourings", "Alignment", "Colour scheme of the alignment", classes.get(0), classes);
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
				
				// TODO
				
				
			}
				
		}catch(Exception e){
			e.printStackTrace();
			return getErrorJSON(e);
		}
		
		
		return "{}";
		
	}
	
	
	/**
	 * Generate all objects. Now ready - to render onto the svg
	 * Return canvas width and height but do not return any of the objects until getGraphics is called
	 * @return
	 */
	public static String initGraphics() {
		
		
		if (!isReady()) return "{}";
		
		
		try {
		
			// Bounds
			double xdivide1 = ((NumericalOption)division1).getVal();
			double xdivide2 = ((NumericalOption)division2).getVal();
			
			JSONObject json = new JSONObject();
			double width = ((NumericalOption)canvasWidth).getVal();
			double height = ((NumericalOption)canvasHeight).getVal();
			
			
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
			
			
			JSONArray objs = new JSONArray();
			
			
			double ntWidth = ((NumericalOption)siteDim).getVal();
			if (ntWidth > 0) {
				height = ntWidth * AlignmentAPI.getNtaxa();
			}
			
			// Taxa?
			if (AlignmentAPI.isReady()) {
				
				double x0 = xdivide1*width + ((NumericalOption)taxaSpacing).getVal();
				
				JSONArray taxa = AlignmentAPI.getTaxaGraphics(x0, xdivide2*width, 0, height);
				objs.putAll(taxa);
				
			}
			
			// Alignment?
			if (AlignmentAPI.isReady()) {
				
				
				JSONArray alignment = AlignmentAPI.getAlignmentGraphics(xdivide2*width, width, 0, height);
				objs.putAll(alignment);
				
				json.put("nsites", AlignmentAPI.getNsites());
				json.put("nsitesdisplayed", AlignmentAPI.getNsitesDisplayed());
				json.put("ntaxa", AlignmentAPI.getNtaxa());
				
				
			}
			
			graphicalObjects = objs;
			
			System.out.println(json.toString());
			
			return json.toString();
		
		} catch (Exception e) {
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
		// TODO use a string builder for efficiency
		String out = "[";
		int len = 0;
		boolean addedObj = false;
		do {
			String str = graphicalObjects.get(0).toString();
			if (str.length() + len < CHUNK_SIZE) {
				
				//System.out.println("Added json at position " + i + ". Total length " + );
				
				// Update string
				if (addedObj) out = out + ",";
				addedObj = true;
				out = out + str;
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

		out += "]";
		
		return out;
	}

	
	
	
	/**
	 * Get a list of options
	 * @return
	 * @throws Exception
	 */
	private static List<Option> getOptionList() throws Exception{
		
		prepareColourings();
		
		
		// Introspectively find all options and add them to the list of options
		List<Option> options = new ArrayList<>();
		Field[] fields = OptionsAPI.class.getDeclaredFields();
		for (Field field: fields) {
			
			if (field.getType().equals(Option.class)) {
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
