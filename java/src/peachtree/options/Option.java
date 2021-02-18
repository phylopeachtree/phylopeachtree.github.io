package peachtree.options;


import org.json.JSONObject;

public abstract class Option {
	
	
	String name; // Name of variable
	String section; // Category it belongs to
	String title; // Description

	public String getName() {
		return name;
	}
	
	public String getSection() {
		return section;
	}
	
	public String getTitle() {
		
		return title;
	}
	
	
	public abstract JSONObject toJSON();
	

}
