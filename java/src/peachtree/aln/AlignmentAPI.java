package peachtree.aln;



import java.util.Calendar;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.options.OptionsAPI;
import peachtree.options.Scaling;

public class AlignmentAPI {

	
	private static Alignment THE_ALIGNMENT;
	
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
		
		
		try {
			long start = Calendar.getInstance().getTimeInMillis();
			THE_ALIGNMENT = new Alignment(str);
			long finish = Calendar.getInstance().getTimeInMillis();
			System.out.println("Parsed successfully (" + (finish-start) + "ms)" );
			return "{}";
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
	 * Return a list of site colourings available for the current alignment
	 * @return
	 * @throws Exception
	 */
	public static String getColourings() throws Exception {
		
		
		/*
		 Reflections reflections = new Reflections("java.util");
		    Set<Class<? extends List>> classes = reflections.getSubTypesOf(peatree.aln.Colouring.class);
		    for (Class<? extends List> aClass : classes) {
		        System.out.println(aClass.getName());
		        if(aClass == ArrayList.class) {
		            List list = aClass.newInstance();
		            list.add("test");
		            System.out.println(list.getClass().getName() + ": " + list.size());
		        }
		    }
		*/
		
		return "";
	}


	
	/**
	 * Get the graphics for the alignment within the bounds
	 * @param xmin
	 * @return
	 */
	public static JSONArray getGraphics(double xmin, double xmax, double ymin, double ymax) {
		
		// Switch ymax and ymin for js
		Scaling scaling = new Scaling(0, 1, 0, 1, xmin, xmax, ymin, ymax);
		return THE_ALIGNMENT.getGraphics(scaling);
	}
	
	
}






