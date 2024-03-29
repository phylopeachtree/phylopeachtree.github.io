/*
 * Scaling.cpp
 *
 *  Created on: 6/07/2021
 *      Author: jdou557
 */

#include "Scaling.h"


Scaling::Scaling(double canvasMinX, double canvasMaxX, double canvasMinY, double canvasMaxY) {
	this->canvasMinX = canvasMinX;
	this->canvasMaxX = canvasMaxX;
	this->canvasMinY = canvasMinY;
	this->canvasMaxY = canvasMaxY;
}

Scaling::Scaling(double canvasMinX, double canvasMaxX, double canvasMinY, double canvasMaxY, double xmin, double xmax) {
	this->canvasMinX = canvasMinX;
	this->canvasMaxX = canvasMaxX;
	this->canvasMinY = canvasMinY;
	this->canvasMaxY = canvasMaxY;
	this->xmin = xmin;
	this->xmax = xmax;
}


/*
 * Set relative positions [0,1] of the scroll bars
 * This is assumed to be the left/top of the view (unless the scroll bar is near a terminal)
 * The absolute scroll position is then set as the centre of the scroll bar
 */
void Scaling::setScroll(double x, double y, double fullWidth, double fullHeight){
	this->fullWidth = fullWidth;
	this->fullHeight = fullHeight;


	// X scroll
	this->scrollX = 0;
	if (x > 0) {


		if (x > 1) x = 1;

		double viewWidth = (this->canvasMaxX - this->canvasMinX);
		double scrollXLen = this->getScrollXLength();
		this->scrollX = x * (fullWidth-viewWidth);

		//cout << "x scrolling: " << x << "/" << fullWidth << "/" << viewWidth << "/" << this->scrollX << endl;


	}



	// Y scroll
	this->scrollY = 0;
	if (y > 0) {

		if (y > 1) y = 1;

		double viewHeight = (this->canvasMaxY - this->canvasMinY);
		double scrollYLen = this->getScrollYLength();
		this->scrollY = y * (fullHeight-viewHeight); // + this->canvasMinY;

		//cout << "y scrolling: " << y << "/" << fullHeight << "/" << viewHeight << "/" << this->scrollY << endl;


	}

}


/**
 * Get the length of the y scroll bar
 * @return
 */
double Scaling::getScrollYLength(){
	double viewSize = (this->canvasMaxY - this->canvasMinY);
	if (viewSize >= this->fullHeight) return 0;
	return std::max(viewSize*viewSize / this->fullHeight, MIN_THUMB_SIZE);
}


/**
 * Get the length of the x scroll bar
 * @return
 */
double Scaling::getScrollXLength(){
	double viewSize = (this->canvasMaxX - this->canvasMinX);
	if (viewSize >= this->fullWidth) return 0;
	return std::max(viewSize*viewSize / this->fullWidth, MIN_THUMB_SIZE);
}


/**
 * Set the scaled height of a row (eg. a sequence/taxon/horizontal branch)
 * @param rowH
 */
void Scaling::setRowHeight(double rowH){
	this->rowH = rowH;
}


/**
 * Get the scaled row height
 * @return
 */
double Scaling::getRowHeight(){
	return this->rowH;
}


/**
 * Checks whether this row number should be displayed
 * @param rowPos
 * @return
 */
bool Scaling::inRangeY(double rowPos){
	double h = this->scaleY(rowPos);
	if (h < this->canvasMinY) return false;
	if (h - rowH > this->canvasMaxY) return false;
	return true;
}


/**
 * Checks whether the row is too far above the range
 * @param rowPos
 * @return
 */
bool Scaling::isAboveRangeY(double rowPos){
	double h = this->scaleY(rowPos);
	if (h - rowH > this->canvasMaxY) return true;
	return false;
}


/**
 * Gets the y-axis value for an object at this row position (i is sequence/taxon/branch i for i in 0,1,2,n-1)
 * @param rowPos
 * @return
 */
double Scaling::scaleY(double rowPos){
	//cout << rowPos << "->" << this->rowH << "/" << this->canvasMinY << "/" << this->scrollY << endl;
	return rowPos * this->rowH + this->canvasMinY - this->scrollY;
}


/**
 * Check if this unscaled value is in range
 * @param x
 * @return
 */
bool Scaling::inRangeX(double x){
	return this->inRangeX(x, 0);
}

/*
 * Check if this unscaled value, plus or minus a margin, is in range
*/
bool Scaling::inRangeX(double x, double margin){
	double h = (x - xmin) / (xmax - xmin) * (canvasMaxX - canvasMinX) + canvasMinX - this->scrollX;
	if (h < this->canvasMinX) return false;
	if (h > this->canvasMaxX) return false;
	return true;
}


/**
 * Checks whether the xpos is too far above the range
 * @param rowPos
 * @return
 */
bool Scaling::isAboveRangeX(double x){
	return isAboveRangeX(x, 0);
}


/*
 * Check whether line (y1,y2) is in the canvas
 */
bool Scaling::isLineInRangeY(double y1, double y2){
	double top = this->scaleY(y1);
	double btm = this->scaleY(y2);
	
	if (top > btm){
		double tmp = top;
		top = btm;
		btm = tmp;
	}
	
	// One end is visible
	if (top >= this->canvasMinY && top <= this->canvasMaxY) return true;
	if (btm >= this->canvasMinY && btm <= this->canvasMaxY) return true;
	
	
	// Both ends are invisible, but middle is visible
	if (top < this->canvasMinY && btm > this->canvasMaxY) return true;
	
	
	// Both ends and the connecting segment are invisible
	return false;
	
	
	
}



/**
 * Checks whether the xpos, plus or minus a margin, is too far above the range
 * @param rowPos
 * @return
 */
bool Scaling::isAboveRangeX(double x, double margin){
	double h = this->scaleX(x);
	if (h > this->canvasMaxX) return true;
	return false;
}


/**
 * Scale X
 * @param x
 * @return
 */
double Scaling::scaleX(double x){
	x = (x - xmin) / (xmax - xmin) * (canvasMaxX - canvasMinX) + canvasMinX - this->scrollX;
	return x;
}


double Scaling::getXmax(){
	return this->xmax;
}

double Scaling::getXmin(){
	return this->xmin;
}




double Scaling::getCanvasMinX(){
	return this->canvasMinX;
}

double Scaling::getCanvasMaxX(){
	return this->canvasMaxX;
}

double Scaling::getCanvasMinY(){
	return this->canvasMinY;
}

double Scaling::getCanvasMaxY(){
	return this->canvasMaxY;
}


void Scaling::cleanup(){
	// Nothing to do
}


/**
 * Is the width and height > 0
 * @return
 */
bool Scaling::inView(){
	if (this->canvasMaxX - this->canvasMinX <= 0) return false;
	if (this->canvasMaxY - this->canvasMinY <= 0) return false;
	return true;
}



