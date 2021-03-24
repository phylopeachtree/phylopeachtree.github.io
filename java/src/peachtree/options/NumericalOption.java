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
	double stepSize;
	
	

	public NumericalOption(String name, String section, String title, double val, double min, double max, double stepSize) {
		this(name, section, title, val, min, max, stepSize, false);
	}
	
	
	public NumericalOption(String name, String section, String title, double val, double min, double max, double stepSize, boolean hide) {
		
		this.name = name;
		this.min = min;
		this.max = max;
		this.value = val;
		this.section = section;
		this.title = title;
		this.stepSize = stepSize;
		this.hide = hide;
	}
	
	
	
	
	@Override
	public JSONObject toJSON() {
		JSONObject json = new JSONObject().put("value", value).put("min", min).put("max", max).put("step", stepSize);
		super.modifyJSON(json);
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

	
	/**
	 * Unhide the option
	 */
	public void show() {
		this.hide = false;
	}

	/**
	 * Hide the option
	 */
	public void hide() {
		this.hide = true;
	}
	
	
}









