package peachtree.options;

import org.json.JSONObject;



/**
 * Numerical option with a min and max val
 * @author Jordan
 *
 */
public class NumericalOption extends Option {

	
	double value;
	double min;
	double max;
	
	
	public NumericalOption(String name, String section, String title, double val, double min, double max) {
		
		this.name = name;
		this.min = min;
		this.max = max;
		this.value = val;
		this.section = section;
		this.title = title;
		
	}
	
	
	@Override
	public JSONObject toJSON() {
		JSONObject json = new JSONObject().put("name", name).put("section", section).put("title", title)
						.put("value", value).put("min", min).put("max", max).put("type", this.getClass().getSimpleName());
		return json;
	}
	
	public double getVal() {
		return this.value;
	}
	
	public void setVal(double val) {
		if (Double.isNaN(val)) return;
		if (val < this.min) val = this.min;
		if (val > this.max) val = this.max;
		this.value = val;
	}

}
