package peachtree.options;


import org.json.JSONObject;

public abstract class Option {
	
	
	String name; // Name of variable
	String section; // Category it belongs to
	String title; // Description
	boolean hide; // Hide it?

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
	
	
	/**
	 * Add constants to the json
	 * @param json
	 */
	protected void modifyJSON(JSONObject json) {
		json.put("name", name).put("section", section).put("title", title).put("type", this.getClass().getSimpleName()).put("hide", hide);
	}
	

}
