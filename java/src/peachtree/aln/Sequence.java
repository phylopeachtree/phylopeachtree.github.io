package peachtree.aln;


import org.json.JSONArray;
import org.json.JSONObject;


import peachtree.aln.colourings.Colouring;
import peachtree.options.Scaling;

public class Sequence {
	
	
	

	Taxon taxon;
	StringBuilder sequence;
	int[] sequenceArr;
	int seqLen;
	boolean isNucleotide;
	

	
	
	public Sequence(int id, String acc, StringBuilder seq, boolean couldBeNucleotide) throws Exception {
		
		
		this.taxon = new Taxon(id, acc);
		this.sequenceArr = null;
		
		this.sequence = seq;
		
		// Is it nucleotide or amino acid?
		this.isNucleotide = true;
		seqLen = seq.length();
		if (couldBeNucleotide) {
			this.sequenceArr = new int[this.seqLen];
			String c;
			for (int i = 0; i < seq.length(); i ++) {
				
				
				c = Character.toString(seq.charAt(i));
						//seq.substring(i, i+1);
				Integer val = Alignment.nt_chars.get(c);
				if (val == null) {
					this.isNucleotide = false;
					return;
				}
				
				this.sequenceArr[i] = val;
			}
		}
		
	}
	
	
	
	public Sequence copy() {
		try {
			Sequence seq = new Sequence(this.getTaxon().getID(), this.getAcc(), new StringBuilder(this.sequence.toString()), this.isNucleotide);
			seq.isNucleotide = this.isNucleotide;
			if (!this.isNucleotide) seq.prepareArray();
			return seq;
		} catch(Exception e) {
			e.printStackTrace();
			return null;
		}
	}
	
	
	
	
	/**
	 * Prepare int array of the sequence
	 */
	public void prepareArray() throws Exception {
		//System.out.println("prepare array " + this.isNucleotide + " length " + this.sequence.length() + "/" + this.seqLen);
		//System.out.println("'" + this.sequence + "'");
		this.sequenceArr = new int[this.seqLen];
		for (int i = 0; i < this.seqLen; i ++) {
			String site = this.sequence.substring(i, i+1);
			Integer val;
			if (this.isNucleotide) {
				val = Alignment.nt_chars.get(site);
			}else {
				val = Alignment.alpha_chars.get(site);
			}
			//System.out.println("putting " + val + " at site " + i + " for symbol " + site);
			if (val == null) {
				throw new Exception("Unknown character " + site + " at site " + (i+1) + " in " + this.getAcc());
			}
			this.sequenceArr[i] = val;
		}
	}
	


	public StringBuilder getSeq() {
		return this.sequence;
	}
	
	
	public Taxon getTaxon() {
		return this.taxon;
	}
	
	public String getAcc() {
		return this.taxon.getName();
	}
	
	public int getID() {
		return this.taxon.getID();
	}
	
	public JSONObject toJSON() {
		JSONObject json = new JSONObject().put("acc",  this.getAcc()).put("seq", this.sequence);  
		return json;
	}

	public int getLength() {
		return this.seqLen;
	}

	public boolean isNucleotide() {
		return this.isNucleotide;
	}

	public void setIsNucleotide(boolean b) {
		this.isNucleotide = b;
	}

	
	
	/**
	 * Get taxon graphics
	 * @param scaling
	 * @param y
	 * @param height
	 * @param filtering
	 * @return
	 */
	public JSONArray getTaxonGraphics(Scaling scaling, int seqNum, int padding, Filtering filtering, double textSize, boolean showTaxonNumbers, Double[] yshift) {
		
		JSONArray arr = new JSONArray();
		
		// Do not plot beyond the edge
		if (!scaling.inRangeY(seqNum) || textSize <= 0) return arr;
		
		
		// Y shift
		if (yshift[0] == null) {
			yshift[0] = scaling.canvasMinY() - scaling.scaleY(seqNum);
		}
		double yc_scaled = scaling.scaleY(seqNum + 0.5) + yshift[0];
		
		String numberPadding = showTaxonNumbers ? padRight((seqNum+1)+":", padding+2) : "";
		numberPadding.replaceAll(" ", "&#160;"); // White space
		
		
		// Plot accession
		JSONObject acc_json = new JSONObject();
		acc_json.put("ele", "text").put("x", scaling.scaleX(scaling.xmin())).put("y", yc_scaled);
		acc_json.put("text_anchor", "start"); 
		acc_json.put("value", numberPadding + this.getAcc());
		acc_json.put("title", this.getAcc());
		acc_json.put("font_size", textSize);
		acc_json.put("white_space", "pre");
		acc_json.put("class", "taxon" + (this.getTaxon().isSelected() ? " selected" : "") );
		acc_json.put("i", seqNum);
		
		
		arr.put(acc_json);
		
		
		return arr;
		
	}
	
	public static String padRight(String s, int n) {
	     return String.format("%-" + n + "s", s);  
	}

	
	
	/**
	 * Create JSON object of all elements in this sequence
	 * @param xmin
	 * @param xmax
	 * @param y
	 * @return
	 */
	public JSONArray getSequenceGraphics(Scaling scaling, int seqNum, double ntWidth, Colouring colouring, Filtering filtering, double textSize, Double[] yshift) {
		
		JSONArray arr = new JSONArray();
		
		// Do not plot beyond the edge
		if (!scaling.inRangeY(seqNum)) return arr;
		
		
		double x = 0;
		
		double heightScaled = scaling.getRowHeight() + 1;
		//double x = scaling.xmin();
		

		// Y shift to avoid clipping the top margin
		double yc_rect_scaled = scaling.scaleY(seqNum);
		double yc_text_scaled = scaling.scaleY(seqNum + 0.5);
		if (yshift[0] == null) {
			yshift[0] = scaling.canvasMinY() - yc_rect_scaled;
		}
		yc_rect_scaled += yshift[0];
		yc_text_scaled += yshift[0];
		
		
		
		// Add some xshift to the first nucleotide so it doesn't get clipped by left boundary
		Double xshift = null;
		
		
		// Plot sites
		JSONObject nt_bg, nt_font;
		String colour;
		for (int site : filtering.getSites()) {

			// Do not plot beyond the edge
			if (!scaling.inRangeX(x, ntWidth)) {
				x ++;
				continue;
			}
			if (scaling.isAboveRangeX(x, ntWidth)) break;
			
			String symbol = this.getSymbol(site);
			
			
			
			double xc = scaling.scaleX(x);
			if (xshift == null) {
				xshift = scaling.canvasMinX() - xc;
			}
			xc += xshift;
			
			
			
			
			// Background colour
			nt_bg = new JSONObject().put("ele", "rect").put("x", xc).put("y", yc_rect_scaled)
								.put("width", ntWidth+1).put("height", heightScaled);
			if (colouring != null) {
				colour = colouring.getColour(symbol, site);
				nt_bg.put("fill", colour);
				nt_bg.put("stroke", colour);
			}
			//nt_bg.put("fill", "#008cba");
			//nt_bg.put("color", "white");
			
			
			
			arr.put(nt_bg);
			
			
			// Text
			if (textSize > 0) {
				xc = scaling.scaleX(x + 0.5) + xshift;
				nt_font = new JSONObject().put("ele", "text").put("x", xc).put("y", yc_text_scaled);
				nt_font.put("value", symbol);
				nt_font.put("text_anchor", "middle"); // Right alignment
				nt_font.put("title", "Site " + (site+1));
				nt_font.put("font_size", textSize);
				arr.put(nt_font);
			}
			
			x ++;
			
		}
		
		
		return arr;
		
	}

	
	public int getSymbolInt(int site) {
		return this.sequenceArr[site];
	}
	

	public String getSymbol(int site) {
		int i = this.getSymbolInt(site);
		String val;
		if (this.isNucleotide) {
			val = Alignment.nt_ids[i];
		}else {
			val = Alignment.alpha_ids[i];
		}
		
		//System.out.println(site + "->" + i + "->" + val);
		
		if (val == null) return "?";
		return val;
	}


	
	/**
	 * Change the site
	 * @param siteNum
	 * @param char1
	 */
	public void editSiteInt(int siteNum, int newSymbol) {
		this.sequenceArr[siteNum] = newSymbol;
	}
	
	
	
	

}







