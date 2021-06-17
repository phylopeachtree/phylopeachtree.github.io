package peachtree.epi;

import java.util.Calendar;
import java.util.List;

import org.json.JSONObject;

import peachtree.options.OptionsAPI;

public class EpiAPI {
	
	
	private static Epidemiology EPIDEMIOLOGY = null;
	
	
	/**
	 * Upload epidmiological information as a csv/tsv
	 * @param contents
	 * @return
	 */
	public static String uploadEpi(String contents) {
		
		System.out.println("Uploading epidemiological information " + contents.length());
		
		
		try {
			long start = Calendar.getInstance().getTimeInMillis();
			
			
			EPIDEMIOLOGY = new Epidemiology();
			EPIDEMIOLOGY.parseFile(contents);
			
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


	public static List<String> getAllAnnotations() {
		if (EPIDEMIOLOGY == null) return null;
		return EPIDEMIOLOGY.getAnnotations();
	}

}
