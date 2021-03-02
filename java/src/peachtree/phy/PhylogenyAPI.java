package peachtree.phy;

import java.util.Calendar;

import org.json.JSONObject;

import peachtree.aln.Alignment;
import peachtree.options.OptionsAPI;
import peachtree.phy.util.LinkType;

public class PhylogenyAPI {

	
	private static Tree THE_TREE;
	
	
	public static int uploadTree(String contents) {
		
		System.out.println("Uploading tree");
		
		
		return 1;
		
	}
	
	
	
	public static String buildTree(Alignment alignment, LinkType method) {
		
		
		//String str = new String(contents);
		System.out.println("Creating tree " + method.toString());
		
		
		try {
			long start = Calendar.getInstance().getTimeInMillis();
			
			THE_TREE = new ClusterTree(alignment, method);
			
			
			long finish = Calendar.getInstance().getTimeInMillis();
			System.out.println("Built successfully (" + (finish-start) + "ms)" );
			
			JSONObject json = new JSONObject().put("time", (finish-start)).put("newick", THE_TREE.toNewick());
			return json.toString();
			
		}catch (Exception e){
			e.printStackTrace();
			return OptionsAPI.getErrorJSON(e);
		}
		
		
	}
	
	
	
	
	
}
