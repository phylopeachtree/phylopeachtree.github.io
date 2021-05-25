package peachtree;

import java.io.BufferedReader;
import java.io.FileReader;

import peachtree.aln.AlignmentAPI;
import peachtree.options.OptionsAPI;
import peachtree.phy.PhylogenyAPI;
import peachtree.phy.Tree;

public class MainApplication {

	
	
	
	
	public static void main(String[] args) throws Exception {
		
		
		System.out.println("Initialising peatree...");
		if (false) {
			
			BufferedReader reader = new BufferedReader(new FileReader("/home/jdou557/Documents/Marsden2019/BorderIncursions/alignments/seqs/aln.fasta"));
			StringBuilder stringBuilder = new StringBuilder();
			String line = null;
			String ls = System.getProperty("line.separator");
			while ((line = reader.readLine()) != null) {
				stringBuilder.append(line);
				stringBuilder.append(ls);
			}
			stringBuilder.deleteCharAt(stringBuilder.length() - 1);
			reader.close();
			
			
			String fasta = stringBuilder.toString();
			AlignmentAPI.uploadAlignment(fasta);
	
			
		
			reader = new BufferedReader(new FileReader("/home/jdou557/Documents/Marsden2019/BorderIncursions/results/borderIncursion/combined/mcc.nexus"));
			line = null;
			ls = System.getProperty("line.separator");
			while ((line = reader.readLine()) != null) {
				stringBuilder.append(line);
				stringBuilder.append(ls);
			}
			stringBuilder.deleteCharAt(stringBuilder.length() - 1);
			reader.close();
			
			String nexus = stringBuilder.toString();
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);
			PhylogenyAPI.uploadTree(nexus);

		}
		
		
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
