/*
 * Timeline.cpp
 *
 *  Created on: 15/07/2021
 *      Author: jdou557
 */

#include "Timeline.h"
#include "../Utils.h"
#include "../phy/Node.h"

const vector<string> Timeline::dateFormats({"yyyy-mm-dd", "dd-mm-yyyy", "mm-dd-yyyy"});
const double Timeline::AXIS_MARGIN = 6.0;
const int Timeline::MAX_NDATES = 24;


Timeline::Timeline(Tree* tree, Epidemiology* epidemiology, string sampleDateVariable, string dateFormat) {

	this->epidemiology = epidemiology;
	this->setSampleDateVariable(sampleDateVariable, dateFormat);
	this->sampleSubtree = nullptr;

	// Calculate earliest tip height in tree
	this->earliestTreeLeafHeight = 0;
	this->meanTipHeight_tree = 0;
	for (Node* leaf : tree->getLeavesAsArray()){
		this->earliestTreeLeafHeight = std::max(this->earliestTreeLeafHeight, leaf->getHeight());
		this->meanTipHeight_tree += leaf->getHeight() / tree->getLeafNodeCount();
	}

}



/*
 * Prepare node sample heights for this subtree
 * If the tree has not changed, then do not do any new calculations
 */
void Timeline::prepareNodeSampleHeights(Node* subtree){

	if (this->sampleSubtree != nullptr && this->sampleSubtree == subtree) return;
	this->sampleSubtree = subtree;
	cout << "prepareNodeSampleHeights" << endl;

	this->resetNodeSampleTimes(subtree);

}


/*
 * Reset all node sample times
 */
void Timeline::resetNodeSampleTimes(Node* node){

	node->clearSampleTime();

	// Reset all node dates to nullptr
	for (Node* child : node->getChildren()){
		this->resetNodeSampleTimes(child);
	}

}

/*
 * Date of latest tip
 */
tm Timeline::getLatestDate(){
	return this->latestDate;
}


/*
 * Date of earliest tip
 */
tm Timeline::getEarliestDate(){
	return this->earliestDate;
}



void Timeline::setSampleDateVariable(string sampleDateVariable, string dateFormat){

	cout << "Preparing timeline... " << sampleDateVariable << "/" << dateFormat << endl;

	// Nothing has changed
	if (this->sampleDateVariable == sampleDateVariable && this->dateFormat == dateFormat) return;

	this->caseDates.clear();


	// Check the variable name is valid
	vector<string> annots = this->epidemiology->getAnnotations();
	if (std::count(annots.begin(), annots.end(), sampleDateVariable) == 0) return;


	// Check the date format is valid
	if (std::count(Timeline::dateFormats.begin(), Timeline::dateFormats.end(), dateFormat) == 0) return;

	// Get canonical date format from date format
	string dateFormatCanonicalNew = getCanonicalDateFormat(dateFormat);


	// Get a unique list of (sample) dates
	vector<string> uniqueDates;
	vector<tm> caseDatesNew;

	for (Case* c : epidemiology->getCases()){
		string val = c->getValue(sampleDateVariable);

		// Attempt to parse date
		struct tm tm;
		if (!Utils::parseDate(val, dateFormatCanonicalNew, tm)) {
			cout << "Warning: could not parse date '" << val << "' from " << dateFormatCanonicalNew << endl;
			tm.tm_mon = -1;
			caseDatesNew.push_back(tm);
			continue;
		}

		caseDatesNew.push_back(tm);

		// Unique dates only
		if (std::count(uniqueDates.begin(), uniqueDates.end(), val) == 0) {
			uniqueDates.push_back(val);
		}
	}

	// Set variable
	this->sampleDateVariable = sampleDateVariable;
	this->dateFormat = dateFormat;
	this->dateFormatCanonical = dateFormatCanonicalNew;

	// Need at least 2 unique dates to create a timeline
	if (uniqueDates.size() > 1) {

		this->caseDates = caseDatesNew;
		this->meanTipHeight_cases = 0;

		// Get latest and earliest date
		this->latestDate = this->caseDates.at(0);
		this->earliestDate = this->caseDates.at(0);
		for (tm d : this->caseDates){
			if (!Utils::compareDates(d, this->latestDate)) this->latestDate = d;
			if (Utils::compareDates(d, this->earliestDate)) this->earliestDate = d;
		}

		this->latestTime = Utils::getTimeFromDate(this->latestDate);
		this->earliestTime = Utils::getTimeFromDate(this->earliestDate);


		// Mean tip height
		for (tm d : this->caseDates){
			this->meanTipHeight_cases += (this->latestTime - Utils::getTimeFromDate(d)) / this->caseDates.size();
		}

		cout << "There are " << uniqueDates.size() << " unique dates. The earliest is " << Utils::getTimeFromDate(earliestDate) << " and the latest is " << Utils::getTimeFromDate(latestDate) << endl;

	}


}



/*
 * Is the timeline ready?
 */
bool Timeline::isReady(){
	return this->caseDates.size() > 0;
}


/*
 * The date of this case
 */
tm Timeline::getSampleDateOfCase(int caseNum){
	if (this->caseDates.size() == 0) {
		tm time;
		time.tm_mon = -1;
	}
	return this->caseDates.at(caseNum);

}


/*
 * The forward time of this case, where the year 0BC is at time 0
 */
double Timeline::getSampleTimeOfCase(int caseNum){
	if (this->caseDates.size() == 0) {
		return 0;
	}
	return Utils::getTimeFromDate(this->caseDates.at(caseNum));
}


/*
 * The backwards time of this case (where latest case is at time 0)
 */
double Timeline::getSampleHeightOfCase(int caseNum){
	if (this->caseDates.size() == 0) {
		return 0;
	}
	return this->latestTime - Utils::getTimeFromDate(this->caseDates.at(caseNum));
}


/*
 * The backwards time of this case (where latest case is at time 0)
 */
double Timeline::getSampleHeight(Node* node){


	double sampleTime = 0;
	string sampleDate = node->getAnnotationValue(this->sampleDateVariable);


	// There is already a time
	if (node->hasSampleTime()){
		sampleTime = node->getSampleTime();
	}

	// Calculate time again
	else{


		struct tm tm;
		//struct tm* existingDate = node->getSampleTime();
		if (sampleDate == "" || !Utils::parseDate(sampleDate, this->dateFormatCanonical, tm)) {

			// Scale by max-min
			double h = node->getHeight();
			//double scale = (this->latestTime - this->earliestTime) / this->earliestTreeLeafHeight;
			double scale = this->meanTipHeight_cases / this->meanTipHeight_tree;
			//cout << "scaled node " << h << " into " << h*scale << endl;
			double hprime = h*scale;

			for (Node* child : node->getChildren()){
				double hc = this->getSampleHeight(child);
				if (hc > hprime) hprime = hc;
			}

			return hprime;
		}

		sampleTime = Utils::getTimeFromDate(tm);
		node->setSampleTime(sampleTime);

	}

	//cout << node->getAcc() << "|" << sampleDate << "| has height " << (this->latestTime - Utils::getTimeFromDate(tm)) << endl;
	return this->latestTime - sampleTime;

}


/*
 * Default
 */
string Timeline::getDefaultDateFormat(){
	return Timeline::dateFormats.at(0);
}



/*
 * Convert one of the elements in dateFormats into a canonical form eg. %Y-%m-%d
 */
string Timeline::getCanonicalDateFormat(string dateFormat){
	if (dateFormat == "yyyy-mm-dd") return "%Y-%m-%d";
	if (dateFormat == "dd-mm-yyyy") return "%d-%m-%Y";
	if (dateFormat == "mm-dd-yyyy") return "%m-%d-%Y";
	return "";
}


/*
 * Cleanup
 */
void Timeline::cleanup(){


}



/*
 * Get the json object encoding the timeline x-axis
 */
jsonObject Timeline::getTimelineGraphics(Node* subtree, Scaling* scaling, double axisFontSize,
										string symptomDateVar, int infectiousDaysBefore, int infectiousDaysAfter){

	jsonObject arr = json::array();


	if (axisFontSize <= 0) return arr;


	cout << "plotting timeline" << endl;


	// x-axis line at bottom (ignore scroll)
	/*
	jsonObject axis;
	axis["ele"] = "line";
	axis["x1"] = scaling->getCanvasMinX(); // + AXIS_MARGIN;
	axis["x2"] = scaling->getCanvasMaxX(); // - AXIS_MARGIN;
	axis["y1"] = scaling->getCanvasMaxY() - AXIS_MARGIN;
	axis["y2"] = scaling->getCanvasMaxY() - AXIS_MARGIN;
	axis["stroke_width"] = 1;
	axis["stroke"] = "red";
	axis["stroke_linecap"] = "round";
	arr.push_back(axis);
	*/


	// Date of the root
	double height = this->getSampleHeight(subtree);


	// First and last date
	tm firstDate = Utils::addYears(this->latestDate, -height);



	// Infectious period: check the variable name is valid
	vector<string> annots = this->epidemiology->getAnnotations();
	if (std::count(annots.begin(), annots.end(), symptomDateVar) > 0) {

		vector<Node*> leaves;
		subtree->getLeafSet(leaves);


		// Calculate yshift to avoid clipping top margin
		double yshift = 0;
		for (int leafNr = 0; leafNr < leaves.size(); leafNr ++) {
			int filteredNr = leaves.at(leafNr)->getFilteredNr();
			if (filteredNr >= 0 && scaling->inRangeY(filteredNr)){
				yshift = scaling->getCanvasMinY() - scaling->scaleY(filteredNr);
				break;
			}
		}

		// Plot the symptom dates
		for (Node* node : leaves){

			string val = node->getAnnotationValue(symptomDateVar);

			// Attempt to parse date
			struct tm symptomDate;
			if (!Utils::parseDate(val, dateFormatCanonical, symptomDate)) {
				cout << "Warning: could not parse symptom date '" << val << "' from " << dateFormatCanonical << endl;
				continue;
			}


			// Get x coords
			struct tm infectiousStartDate = Utils::addDays(symptomDate, -infectiousDaysBefore);
			struct tm infectiousEndDate = Utils::addDays(symptomDate, infectiousDaysAfter);
			double symptomX = scaling->scaleX(this->latestTime - Utils::getTimeFromDate(symptomDate));
			double infectiousStartX = scaling->scaleX(this->latestTime - Utils::getTimeFromDate(infectiousStartDate));
			double infectiousEndX = scaling->scaleX(this->latestTime - Utils::getTimeFromDate(infectiousEndDate));

			// Get y coord
			double y = node->getFilteredNr() + 0.4;
			double yscaled = scaling->scaleY(y) + yshift;

			const int nodeRadius = 5;



			// Plot infectious period
			jsonObject tick;
			tick["ele"] = "line";
			tick["x1"] = infectiousStartX;
			tick["x2"] = infectiousEndX;
			tick["y1"] = yscaled;
			tick["y2"] = yscaled;
			tick["stroke_width"] = 4;
			tick["stroke"] = "#F7941Daa";
			tick["stroke_linecap"] = "round";
			//tick["class"] = "symptom";
			//tick["title"] = node->getAcc() + " developed symptoms on " + Utils::formatDate(symptomDate);
			arr.push_back(tick);


			// Plot an orange circle on symptom date
			jsonObject node_json;
			node_json["ele"] = "circle";
			node_json["cx"] = symptomX;
			node_json["cy"] = yscaled;
			node_json["r"] = nodeRadius;
			node_json["fill"] = "#F7941Daa";
			node_json["class"] = "symptom";
			node_json["title"] = node->getAcc() + " developed symptoms on " + Utils::formatDate(symptomDate);
			arr.push_back(node_json);


		}


	}



	// Get some nice dates to print
	int ndates = std::floor((scaling->getCanvasMaxX() - scaling->getCanvasMinX()) / (5 * axisFontSize));
	ndates = std::min(ndates, Timeline::MAX_NDATES);
	if (ndates > 0){


		vector<double> heights;
		vector<string> labels;
		Utils::getNiceTimes(this->latestDate, height, ndates, heights, labels);

		// Plot the axis lines
		for (int t = 0; t < heights.size(); t ++){


			// x value
			double x = heights.at(t);
			if (!scaling->inRangeX(x)) continue;

			double x_scaled = scaling->scaleX(x);
			string label = labels.at(t);


			// Vertical line
			jsonObject tick;
			tick["ele"] = "line";
			tick["x1"] = x_scaled;
			tick["x2"] = x_scaled;
			tick["y1"] = scaling->getCanvasMaxY();
			tick["y2"] = scaling->getCanvasMinY();
			tick["stroke_width"] = 1;
			tick["stroke"] = "#29465b";
			tick["stroke_linecap"] = "round";
			arr.push_back(tick);




			//cout << "tick " << time << " | " << x << " | " << x_scaled << endl;

		}

		// Plot the axis labels on top
		for (int t = 0; t < heights.size(); t ++){


			// x value
			double x = heights.at(t);
			if (!scaling->inRangeX(x)) continue;

			double x_scaled = scaling->scaleX(x);
			string label = labels.at(t);


			// A background rectangle under the text. Assuming Courier new font with a width height ratio of 0.56
			jsonObject bg_json;
			bg_json["ele"] = "rect";
			bg_json["x"] = x_scaled + AXIS_MARGIN;
			bg_json["y"] = scaling->getCanvasMaxY() - AXIS_MARGIN - axisFontSize/2.0;
			bg_json["width"] = axisFontSize*0.56*label.size();
			bg_json["height"] = axisFontSize;
			bg_json["fill"] = "#ffffffdd";
			arr.push_back(bg_json);


			// Label
			jsonObject label_json;
			label_json["ele"] = "text";
			label_json["x"] = x_scaled + AXIS_MARGIN;
			label_json["y"] = scaling->getCanvasMaxY() - AXIS_MARGIN;
			label_json["text_anchor"] = "start";
			label_json["alignment_baseline"] = "middle";
			label_json["value"] = label;
			label_json["font_size"] = axisFontSize;
			label_json["font_family"] = "Arial";
			arr.push_back(label_json);


		}


	}

	return arr;

}

























