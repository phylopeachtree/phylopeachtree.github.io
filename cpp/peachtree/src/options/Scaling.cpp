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

		double viewWidth = (this->canvasMaxX - this->canvasMinX);
		this->scrollX = x * fullWidth + this->canvasMinX;


		// Left side
		double scrollXLen = this->getScrollXLength();
		if (this->scrollX <= scrollXLen) {
			this->scrollX = 0;
		}

		// Right side
		else if (this->scrollX >= fullWidth - viewWidth) {
			this->scrollX = fullWidth - viewWidth;
		}

		// In between
		else {
			this->scrollX += scrollXLen/2;
		}



	}


	// Y scroll
	this->scrollY = 0;
	if (y > 0) {

		double viewHeight = (this->canvasMaxY - this->canvasMinY);
		this->scrollY = y * fullHeight + this->canvasMinY;

		// Top of window
		double scrollYLen = this->getScrollYLength();
		if (this->scrollY <= scrollYLen) {
			this->scrollY = 0;
		}

		// Bottom of window
		else if (this->scrollY >= fullHeight - viewHeight) {
			this->scrollY = fullHeight - viewHeight;
		}

		// In between
		else {
			this->scrollY += scrollYLen/2;
		}

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
	if (h + margin < this->canvasMinX) return false;
	if (h - margin > this->canvasMaxX) return false;
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


/**
 * Checks whether the xpos, plus or minus a margin, is too far above the range
 * @param rowPos
 * @return
 */
bool Scaling::isAboveRangeX(double x, double margin){
	double h = this->scaleX(x);
	if (h - margin > this->canvasMaxX) return true;
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



