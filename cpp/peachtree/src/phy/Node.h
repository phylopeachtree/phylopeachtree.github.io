/*
 * Node.h
 *
 *  Created on: 6/07/2021
 *      Author: jdou557
 */

#ifndef PHY_NODE_H_
#define PHY_NODE_H_

#include <iostream>
#include <map>
#include "../lib/json.hpp"
#include "../aln/Taxon.h"
#include "../options/Scaling.h"
#include "../aln/Filtering.h"
#include "../epi/Case.h"


using namespace std;
using jsonObject = nlohmann::json;

class Timeline;
class Node {
public:
	Node();
	Node(int index);
	string getAcc();
	void setAcc(string acc);
	void setTaxon(Taxon* taxon);
	Taxon* getTaxon();
	int getNr();
	void setNr(int nr);
	void setFilteredNr(int nr);
	int getFilteredNr();
	double getHeight();
	double getHeight(Timeline* timeline);
	double getYoungestChildHeight();
	double getYoungestChildHeight(Timeline* timeline);
	void setHeight(double h);
	Node* getParent();
	void addChild(Node* child);
	bool isLeaf();
	bool isRoot();
	Node* getChild(int i);
	int getNumChildren();
	vector<Node*> getChildren();
	Node* copy();
	int getNodeCount();
	int getLeafNodeCount();
	void getLeafSet(vector<Node*>& leaves);
	int getChildCount();
	double getLength();
	string tostring();
   	string toSortedNewick(int& maxNodeInClade, bool printMetaData, vector<string>* labels);
	double getGraphics(bool isRoot, jsonObject& objs, Filtering* filtering, Scaling* scaling, double branchWidth,
			bool showTaxaOnTree, double yshift, double nodeRadius, string branchColourBy, string nodeColourBy, double fontSize, int rounding,
			bool transmissionTree, Timeline* timeline, bool displayIncompatibleTransmissions, string branchCol, string nodeCol,
			vector<double>& minMaxNode, vector<double>& minMaxBranch);
	void parseFromNewick(string newick);

	void getAllAnnotations(vector<string>& annots);
	string getAnnotationValue(string var);
	void rotateChildren();
	void addAnnotations(Case* c);
	void cleanup();

	double getSampleTime();
	void setSampleTime(double time);
	void clearSampleTime();
	bool hasSampleTime();

	bool getIsCompatibleTransmissionEvent();
	void setIsCompatibleTransmissionEvent(bool val);
	void getMinMax(string var, vector<double>& minMax);
	string getAnnotationColour(string var, double min, double max, string colourMax);
	string getAnnotationColour(double val, double min, double max, string colourMax);
	void reorderTransmissions(Timeline* timeline, string symptomDateVar);
	void countInfections();
	void resetInfections();
	void normaliseInfections(int numTrees);
	Node* getFirstLeaf();


private:

	string getNewickMetaData();
	string getTidyMetaData(Timeline* timeline);
	void parseAnnotation(string annotation);

	int nodeNr;
	int filteredNodeNr;
	string acc;
	double height;
	double sampleTime;
	bool isCompatibleTransmissionEvent;
	Node* parent;
	vector<Node*> children;
	Taxon* taxon;
	map<string, string> annotations;


protected:
	void setParent(Node* parent);
	void removeParent();


};

#endif /* PHY_NODE_H_ */








