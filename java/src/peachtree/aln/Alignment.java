package peachtree.aln;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.aln.colourings.Colouring;
import peachtree.aln.colourings.Jalview;
import peachtree.options.Scaling;
import peachtree.phy.Node;
import peachtree.phy.Tree;

public class Alignment {
	
	
	List<Sequence> sequences;
	boolean isNucleotide;
	int alignmentLength;
	List<int[]> patterns;
	List<Double> patternWeights;
	
	Filtering filtering;
	
	
	
	static public Map<String, Integer> nt_chars = null;
	static public String[] nt_ids;
	static public Map<String, Integer> alpha_chars = null;
	static public String[] alpha_ids;
	static private int ambiguousNtIndex = -1;
	static private int ambiguousAlphaIndex = -1;

	
	public Alignment(String fasta) throws Exception{
		
		
		if (fasta.isEmpty()) throw new Exception("Empty file contents");
		
		if (nt_chars == null) prepareAlignmentChars();
		
		this.sequences = new ArrayList<>();
		this.alignmentLength = -1;
		this.isNucleotide = true;

		// Parse fasta
		String[] lines = fasta.split("\n");
		String acc = "";
		boolean parsedSeq = true;
		int seqID = 0;
		
		StringBuilder seq = new StringBuilder();
		
		long timeOnSeqs = 0;
		long timeOnStrbuilding = 0;
		for (String line : lines) {
			
			line = line.replaceAll("\r", "");
			
			//System.out.println(line);
			
			if (line.substring(0,1).equals(">")) {
				
				long t0 = Calendar.getInstance().getTimeInMillis();
				
				if (!parsedSeq) {
					throw new Exception("Cannot find sequence for " + acc);
				}
				
				
				// Parse the sequence
				if (seq.length() > 0) {
					
					parseSequence(acc, seq, seqID);
					seqID++;
					seq = new StringBuilder();
				}
				
				acc = line;
				parsedSeq = false;
				
				long t1 = Calendar.getInstance().getTimeInMillis();
				timeOnSeqs += (t1-t0);
				
			}
			
			
			
			
			else {
				
				long t0 = Calendar.getInstance().getTimeInMillis();
				
				parsedSeq = true;
				seq.append(line.toUpperCase());
				
				long t1 = Calendar.getInstance().getTimeInMillis();
				timeOnStrbuilding += (t1-t0);
			}
			
		}
		
		
		
		// Parse the last sequence
		if (seq.length() > 0) {
			parseSequence(acc, seq, seqID);
		}
		
		
		if (this.sequences.isEmpty()) {
			throw new Exception("Cannot detect any sequences in the alignment. Are you sure this is .fasta?");
		}
		
		
		// Build all sequence arrays
		for (Sequence sequence : sequences) {
			sequence.prepareArray();
		}
		
		
		long t1 = Calendar.getInstance().getTimeInMillis();
		
		
		// Default filtering
		this.filtering = new Filtering(true, null, this);
		
		
		// Initialise patterns
		this.initPatterns();
		
		System.out.println("Parsed an alignment with " + this.alignmentLength + " sites, " + this.getPatternCount() + " patterns, and " + this.sequences.size() + " taxa ");
		
		
		long timeOnFilteringT = Calendar.getInstance().getTimeInMillis() - t1;
		System.out.println(timeOnFilteringT + "ms on filtering");
		System.out.println(timeOnSeqs + "ms on seq");
		System.out.println(timeOnStrbuilding + "ms on str");
		
		
	}
	
	
	/**
	 * Initialise site patterns
	 */
	private void initPatterns() {
		
		
		this.patterns = new ArrayList<>();
		this.patternWeights = new ArrayList<>();
		for (int siteNum = 0; siteNum < this.getLength(); siteNum ++) {
			
			// Get pattern of this column
			int[] site = new int[this.getNtaxa()];
			for (int taxonNum = 0; taxonNum < site.length; taxonNum++) {
				Sequence sequence = this.getSequence(taxonNum);
				site[taxonNum] = sequence.getSymbolInt(siteNum);
			}
			
			
			// Check if it is unique
			int patternMatch = -1;
			for (int patternNum = 0; patternNum < this.patterns.size(); patternNum ++) {
				int[] pattern = this.patterns.get(patternNum);
				boolean isUniqueFromThisPattern = false;
				for (int taxonNum = 0; taxonNum < site.length; taxonNum++) {
					if (site[taxonNum] != pattern[taxonNum]) {
						isUniqueFromThisPattern = true;
						break;
					}
				}
				
				// This column is a duplicate. Do not add it to the list of unique patterns
				if (!isUniqueFromThisPattern) {
					patternMatch = patternNum;
					break;
				}
				
			}
			
			
			// If unique site, then add to list
			if (patternMatch == -1) {
				this.patterns.add(site);
				this.patternWeights.add(1.0);
			}
			
			// Otherwise increment the pattern weight
			else {
				double weight = this.patternWeights.get(patternMatch);
				this.patternWeights.set(patternMatch, weight + 1.0);
			}
			
		}
		
		
		System.out.println("There are " + this.patterns.size() + " patterns");
		
		
	}
	
	
	/**
	 * Add the sequence + accession
	 * @param acc
	 * @param seq
	 * @throws Exception 
	 */
	private void parseSequence(String acc, StringBuilder seq, int seqID) throws Exception {
		
		Sequence sequence = new Sequence(seqID, acc, seq);
		if (this.sequences.isEmpty()) {
			this.alignmentLength = sequence.getLength();
			this.isNucleotide = sequence.isNucleotide();
		}else {
			
			// All sequences must have same length
			if (sequence.getLength() != alignmentLength) {
				throw new Exception("Cannot parse sequence " + sequence.getAcc() + " because of length mismatch (" + 
						sequence.getLength() + " != " + alignmentLength + ")");
			}
			
			// Is it still a nucleotide alignment?
			if (this.isNucleotide && !sequence.isNucleotide()) {
				this.isNucleotide = false;
				for (Sequence s : sequences) {
					s.setIsNucleotide(false);
				}
			}
			
			
			// Does it have a unique accession?
			for (Sequence other : sequences) {
				if (other.getAcc().equals(sequence.getAcc())) {
					throw new Exception("Duplicate sequence accession detected: " + sequence.getAcc());
				}
			}
			
		}
		
		sequences.add(sequence);
		
	}
	
	
	/**
	 * Sort the taxa by the tree
	 * @param tree
	 */
	public void sortByTree(Tree tree) throws Exception {
		
		
		Node[] leaves = tree.getLeavesAsArray();
		
		// Find a new ordering
		int[] newOrdering = new int[leaves.length];
		for (int taxonNr = 0; taxonNr < leaves.length; taxonNr ++) {
			
			String accession = leaves[taxonNr].getAcc();
			
			// Find the sequence number with this accession
			int seqIndex = -1;
			for (int i = 0; i < this.getNtaxa(); i++) {
				if (this.getSequence(i).getAcc().equals(accession)) {
					seqIndex = i;
					break;
				}
			}
			if (seqIndex == -1) {
				throw new Exception("Cannot find " + accession + " in alignment");
			}
			
			newOrdering[taxonNr] = seqIndex;
			
		}
		
		
		
		
		// Create new sequence array
		List<Sequence> sequencesNew = new ArrayList<>();
		
		
		// Reorder the array
		for (int newIndex = 0; newIndex < leaves.length; newIndex ++) {
			int oldIndex = newOrdering[newIndex];
			sequencesNew.add(this.getSequence(oldIndex));
			
		}
		
		
		// Point to the new array
		this.sequences = sequencesNew;
		
		// Reinitialise
		this.filtering = new Filtering(true, null, this);
		this.initPatterns();
		
		
	}
	
	
	




	
	/**
	 * Alignment taxon/site filtering
	 * @return
	 */
	public void setFiltering(Filtering filtering) {
		this.filtering = filtering;
	}
	
	public JSONObject toJSON() {
		
		// Array of sequences
		JSONArray arr = new JSONArray();
		for (int i = 0; i < this.sequences.size(); i ++) {
			Sequence seq = this.sequences.get(i);
			arr.put(seq.toJSON());
		}
		
		
		// Alignment
		JSONObject json = new JSONObject().put("alignment",  arr); 
		json.put("length", this.alignmentLength).put("ntaxa", this.sequences.size()).put("isNT", this.isNucleotide);
		return json;
		
		
	}



	/**
	 * Get graphics of the alignment
	 * @return
	 */
	public JSONArray getAlignmentGraphics(Scaling scaling, Colouring colouring, double minNtWidth, double textSize) {
		
		
		
		
		JSONArray objs = new JSONArray();
		
		double dy = (scaling.ymax() - scaling.ymin()) / this.filtering.getNumSeqs();
		double y = scaling.ymin();
		//System.out.println("sequence: " + dy);
		for (Sequence sequence : sequences) {

			
			//long start = Calendar.getInstance().getTimeInMillis();
			
			objs.putAll(sequence.getSequenceGraphics(scaling, y, dy, minNtWidth, colouring, filtering, textSize));
			
			//long finish = Calendar.getInstance().getTimeInMillis();
			//System.out.println("converted sequence " + sequence.getAcc() + " to json (" + (finish - start) + "ms)");
			y += dy;
		}
		
		return objs;
		
		
	}
	
	
	/**
	 * Get taxa graphics
	 * @return
	 */
	public JSONArray getTaxaGraphics(Scaling scaling, double textSize) {
		
		JSONArray objs = new JSONArray();
		
		double dy = (scaling.ymax() - scaling.ymin()) / this.filtering.getNumSeqs();
		double y = scaling.ymin();
		System.out.println("taxa: " + dy);
		for (Sequence sequence : sequences) {
			
			
			//long start = Calendar.getInstance().getTimeInMillis();
			
			objs.putAll(sequence.getTaxonGraphics(scaling, y, dy, filtering, textSize));
			
			//long finish = Calendar.getInstance().getTimeInMillis();
			//System.out.println("converted sequence " + sequence.getAcc() + " to json (" + (finish - start) + "ms)");
			y += dy;
		}
		
		return objs;
		
		
	}

	public int getLength() {
		if (this.sequences == null) return 0;
		return this.alignmentLength;
	}


	public int getNtaxa() {
		if (this.sequences == null) return 0;
		return this.sequences.size();
	}


	public Sequence getSequence(int i) {
		if (this.sequences == null) return null;
		return this.sequences.get(i);
	}
	
	
	/**
	 * Is the index abgisuous / gap?
	 * @param index
	 * @param isNT
	 * @return
	 */
	public static boolean isAmbiguousOrGap(int index, boolean isNT) {
		
		if (isNT) {
			if (index > ambiguousNtIndex) return true;
		} else {
			if (index > ambiguousAlphaIndex) return true;
		}
		return false;
	}
	
	/**
	 * Is the symbol ambiguous or a gap?
	 * @param symbol
	 * @return
	 */
	public static boolean isAmbiguousOrGap(String symbol, boolean isNT) {
		
		
		if (isNT) {
			Integer index = nt_chars.get(symbol);
			if (index == null) return true;
			if (index > ambiguousNtIndex) return true;
		}else {
			Integer index = alpha_chars.get(symbol);
			if (index == null) return true;
			if (index > ambiguousAlphaIndex) return true;
		}
		
		
		return false;
	}
	
	

	
	
	/**
	 * Prepare datatype maps
	 */
	public static void prepareAlignmentChars() {
		
		nt_chars = new HashMap<>();
		
		alpha_chars = new HashMap<>();
		
		// Nucleotide
		int i = -1;
		nt_chars.put("A", ++i);
		nt_chars.put("C", ++i);
		nt_chars.put("G", ++i);
		nt_chars.put("T", ++i);
		nt_chars.put("U", ++i);
		
		// Ambiguous nucleotides / gaps
		ambiguousNtIndex = i;
		nt_chars.put("R", ++i); // Purine (AG)
		nt_chars.put("Y", ++i); // Pyrimidine (TC)
		nt_chars.put("K", ++i); // Keto (GT)
		nt_chars.put("M", ++i); // Amino (AC)
		nt_chars.put("S", ++i); // Strong (GC)
		nt_chars.put("W", ++i); // Weak (AT)
		nt_chars.put("B", ++i); // not A
		nt_chars.put("D", ++i); // not C
		nt_chars.put("H", ++i); // not G
		nt_chars.put("V", ++i); // not T/U
		
		nt_chars.put("N", ++i); // Ambiguous
		nt_chars.put("-", ++i);
		
		// Reverse mapping
		nt_ids = new String[nt_chars.size()];
		for (String key : nt_chars.keySet()) {
			int index = nt_chars.get(key);
			nt_ids[index] = key;
			//System.out.println(key + "->" + index);
		}
		
		
		// General case (including aa)
		i = -1;
		alpha_chars.put("A", ++i);
		alpha_chars.put("C", ++i);
		alpha_chars.put("D", ++i);
		alpha_chars.put("E", ++i);
		alpha_chars.put("F", ++i);
		alpha_chars.put("G", ++i);
		alpha_chars.put("H", ++i);
		alpha_chars.put("I", ++i);
		alpha_chars.put("K", ++i);
		alpha_chars.put("L", ++i);
		alpha_chars.put("M", ++i);
		alpha_chars.put("N", ++i);
		alpha_chars.put("P", ++i);
		alpha_chars.put("Q", ++i);
		alpha_chars.put("R", ++i);
		alpha_chars.put("S", ++i);
		alpha_chars.put("T", ++i);
		alpha_chars.put("V", ++i);
		alpha_chars.put("W", ++i);
		alpha_chars.put("Y", ++i);
		
		// Ambiguous aas / gaps
		ambiguousAlphaIndex = i;
		alpha_chars.put("B", ++i);
		alpha_chars.put("J", ++i);
		alpha_chars.put("O", ++i);
		alpha_chars.put("U", ++i);
		alpha_chars.put("X", ++i);
		alpha_chars.put("Z", ++i);
		alpha_chars.put("-", ++i);

		
		// Reverse mapping
		alpha_ids = new String[alpha_chars.size()];
		for (String key : alpha_chars.keySet()) {
			int index = alpha_chars.get(key);
			alpha_ids[index] = key;
			//System.out.println(key + "->" + index);
		}
		
		
		
	}


	public boolean isNucleotide() {
		return this.isNucleotide;
	}


	/**
	 * Number of sites being displayed
	 * @return
	 */
	public int getNsitesDisplayed() {
		if (this.filtering == null) return 0;
		return this.filtering.getNumSites();
	}


	public List<String> getNames() {
		List<String> names = new ArrayList<>();
		for (Sequence seq : sequences) {
			names.add(seq.getAcc());
		}
		return names;
	}
	
	
	
	/**
	 * Number of site patterns
	 * @return
	 */
	public int getPatternCount() {
		return this.patterns.size();
	}
	
	
	/**
	 * Gets the pattern index patternNum
	 * @param siteNum
	 * @return
	 */
	public int[] getPattern(int patternNum) {
		return this.patterns.get(patternNum);
	}
	
	
	public int getPattern(int patternNum, int taxonNum) {
		return this.patterns.get(patternNum)[taxonNum];
	}


	/**
	 * Weight of pattern i
	 * @param i
	 * @return
	 */
	public double getPatternWeight(int i) {
		return this.patternWeights.get(i);
	}


	/**
	 * Site/taxon filtering
	 * @return
	 */
	public Filtering getFiltering() {
		return this.filtering;
	}




	

}

















