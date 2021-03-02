package peachtree.options;


/**
 * Scale x or y
 * @author Jordan
 *
 */
public class Scaling {

	
	double xmin = 0;
	double xmax = 1;
	double ymin = 0;
	double ymax = 1;
	double canvasMinX;
	double canvasMaxX;
	double canvasMinY;
	double canvasMaxY;

	
	
	public Scaling(double canvasMinX, double canvasMaxX, double canvasMinY, double canvasMaxY) {
		
		this.canvasMinX = canvasMinX;
		this.canvasMaxX = canvasMaxX;
		this.canvasMinY = canvasMinY;
		this.canvasMaxY = canvasMaxY;
		
	}
	
	
public Scaling(double canvasMinX, double canvasMaxX, double canvasMinY, double canvasMaxY, double xmin, double xmax, double ymin, double ymax) {
		
		this.canvasMinX = canvasMinX;
		this.canvasMaxX = canvasMaxX;
		this.canvasMinY = canvasMinY;
		this.canvasMaxY = canvasMaxY;
		
		this.xmin = xmin;
		this.xmax = xmax;
		this.ymin = ymin;
		this.ymax = ymax;
		
	}
	
	
	
	/**
	 * Check if this unscaled value is in range
	 * @param x
	 * @return
	 */
	public boolean inRangeX(double x) {
		return x >= xmin && x < xmax;
	}
	
	/**
	 * Check if this unscaled value is in range
	 * @param x
	 * @return
	 */
	public boolean inRangeY(double y) {
		return y >= ymin && y < ymax;
	}
	
	
	
	/**
	 * Scale X and round 
	 * @param x
	 * @return
	 */
	public double scaleX(double x) {
		x = (x - xmin) / (xmax - xmin) * (canvasMaxX - canvasMinX) + canvasMinX;
		return Math.round(x);
	}
	
	
	/**
	 * Scale Y and round 
	 * @param y
	 * @return
	 */
	public double scaleY(double y) {
		y = (y - ymin) / (ymax - ymin) * (canvasMaxY - canvasMinY) + canvasMinY;
		return Math.round(y);
	}


	public double ymax() {
		return ymax;
	}
	
	public double xmax() {
		return xmax;
	}
	
	public double ymin() {
		return ymin;
	}
	
	public double xmin() {
		return xmin;
	}
	
	
	public double canvasMinX() {
		return canvasMinX;
	}
	
	public double canvasMinY() {
		return canvasMinY;
	}
	
	public double canvasMaxX() {
		return canvasMaxX;
	}
	
	public double canvasMaxY() {
		return canvasMaxY;
	}
	
	
}
