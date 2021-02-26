package peachtree.aln;

import java.util.Calendar;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.aln.colourings.Colouring;
import peachtree.options.Scaling;

public class Sequence {
	
	
	

	Taxon taxon;
	StringBuilder sequence;
	int[] sequenceArr;
	boolean isNucleotide;
	
	
	
	public Sequence(int id, String acc, StringBuilder seq) throws Exception {
		if (acc.substring(0, 1).equals(">")) acc = acc.substring(1);
		
		this.taxon = new Taxon(id, acc);
		this.sequenceArr = null;
		
		this.sequence = seq;
		
		// Is it nucleotide or amino acid?
		this.isNucleotide = true;
		//String[] sites = this.sequence.split("");
		for (int i = 0; i < seq.length(); i ++) {
			
			String c = seq.substring(i, i+1);
			boolean isNT = Alignment.nt_chars.containsKey(c);
			
			if (!isNT) {
				this.isNucleotide = false;
				break;
			}
			
		}
		
		
	}
	
	
	/**
	 * Prepare int array of the sequence
	 */
	public void prepareArray() {
		this.sequenceArr = new int[this.sequence.length()];
		for (int i = 0; i < this.sequenceArr.length; i ++) {
			String site = this.sequence.substring(i, i+1);
			int val;
			if (this.isNucleotide) {
				val = Alignment.nt_chars.get(site);
			}else {
				val = Alignment.alpha_chars.get(site);
			}
			//System.out.println("putting " + val + " at site " + i + " for symbol " + site);
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
		return this.sequence.length();
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
	public JSONArray getTaxonGraphics(Scaling scaling, double y, double height, Filtering filtering) {
		
		JSONArray arr = new JSONArray();
		
		// Should this sequence be included?
		if (filtering != null && !filtering.includeTaxon(this.getTaxon())) return arr;
		
		double yc_text = scaling.scaleY(y + height/2);
		
		// Do not plot beyond the edge
		if (!scaling.inRangeY(yc_text)) return arr;
		
		/// Plot accession
		JSONObject acc_json = new JSONObject();
		acc_json.put("ele", "text").put("x", scaling.scaleX(scaling.xmin())).put("y", yc_text);
		acc_json.put("text_anchor", "start"); 
		acc_json.put("value", this.getAcc());
		acc_json.put("title", this.getAcc());
		arr.put(acc_json);
		
		
		return arr;
		
	}
	
	
	/**
	 * Create JSON object of all elements in this sequence
	 * @param xmin
	 * @param xmax
	 * @param y
	 * @return
	 */
	public JSONArray getSequenceGraphics(Scaling scaling, double y, double height, double minNtWidth, Colouring colouring, Filtering filtering) {
		
		JSONArray arr = new JSONArray();
		
		// Should this sequence be included?
		if (filtering != null && !filtering.includeTaxon(this.getTaxon())) return arr;
		
		
		int seqLen = this.sequence.length();
		int numSites = filtering == null ? seqLen : filtering.getNumSites();
		
		//System.out.println("Displaying " + numSites + " sites");
		
		
		
		double dx = (scaling.xmax() - scaling.xmin()) / numSites;
		double dxScaled = (scaling.scaleX(dx*2) - scaling.scaleX(dx)) + 1; // Add 1 to avoid white space between sites
		if (dxScaled < minNtWidth) {
			numSites = (int) Math.floor((scaling.canvasMaxX() - scaling.canvasMinX()) / minNtWidth);
			dx = (scaling.xmax() - scaling.xmin()) / numSites;
			dxScaled =  (scaling.scaleX(dx*2) - scaling.scaleX(dx)) + 1;
			//System.out.println("nsites = " + numSites + " at width " + minNtWidth + " dx " + dx + " scaled " + dxScaled);
		}
		double heightScaled = scaling.scaleY(height) + 1;
		double x = scaling.xmin();
		double yc_rect = scaling.scaleY(y);
		double yc_text = scaling.scaleY(y + height/2);
		
		if (!scaling.inRangeY(yc_rect)) return arr;
		
		
		
		// Plot sites
		JSONObject nt_bg, nt_font;
		for (int site : filtering.getSites()) {

			
			
			String symbol = this.getSymbol(site);
			
			double xc = scaling.scaleX(x);
			
			
			// Do not plot beyond the edge
			if (!scaling.inRangeX(xc)) break;
			
			
			// Background colour
			nt_bg = new JSONObject().put("ele", "rect").put("x", xc).put("y", yc_rect)
								.put("width", dxScaled).put("height", heightScaled);
			if (colouring != null) {
				nt_bg.put("fill", colouring.getColour(symbol));
			}
			//nt_bg.put("fill", "#008cba");
			//nt_bg.put("color", "white");
			
			
			
			arr.put(nt_bg);
			
			
			// Text
			xc = scaling.scaleX(x + dx/2);
			nt_font = new JSONObject().put("ele", "text").put("x", xc).put("y", yc_text);
			nt_font.put("value", symbol);
			nt_font.put("text_anchor", "middle"); // Right alignment
			nt_font.put("title", "Site " + (site+1));
			
			arr.put(nt_font);
			
			x += dx;
			
			
		}
		
	
		
		
		return arr;
		
	}


	public String getSymbol(int site) {
		int i = this.sequenceArr[site];
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
	
	
	
	

}







