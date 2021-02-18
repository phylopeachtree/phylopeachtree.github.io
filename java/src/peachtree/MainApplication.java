package peachtree;

import peachtree.aln.AlignmentAPI;
import peachtree.options.OptionsAPI;

public class MainApplication {

	
	
	
	
	public static void main(String[] args) {
		
		
		System.out.println("Initialising peatree...");
		
		try {
			OptionsAPI.init();
			AlignmentAPI.init();
			System.out.println("Done!");
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("Error!");
		}
		
		
		
		//Exception e = new Exception("test exception");
		
		//System.out.println(OptionsAPI.getErrorJSON(e));
		
		
	}

}
