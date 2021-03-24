package peachtree;

import peachtree.aln.AlignmentAPI;
import peachtree.options.OptionsAPI;
import peachtree.phy.Tree;

public class MainApplication {

	
	
	
	
	public static void main(String[] args) throws Exception {
		
		
		System.out.println("Initialising peatree...");
		
		
		/*
		Tree tree = new Tree();
		String newick =  "(A[&a=5,b=10]:0.1,(C[&a=5,b=10]:0.3,D[&a=5,b=10]:0.4)[&a=50,b=0]:0.5);";
		newick = "(A[&a=5,b=10]:0.1,B[&a=5,b=10]:0.2,(C[&a=5,b=10]:0.3,D[&a=5,b=10]:0.4)[&a=5,b=10]:0.5);";
		tree.parseFromNexus(newick);
		*/
		
		
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
