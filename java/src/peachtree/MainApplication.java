package peachtree;

import peachtree.aln.AlignmentAPI;
import peachtree.options.OptionsAPI;

public class MainApplication {

	
	
	
	
	public static void main(String[] args) {
		
		
		System.out.println("Initialising peatree...");
		
		OptionsAPI.init();
		AlignmentAPI.init();
		
		
		//Exception e = new Exception("test exception");
		
		//System.out.println(OptionsAPI.getErrorJSON(e));
		
		System.out.println("Done!");
	}

}
