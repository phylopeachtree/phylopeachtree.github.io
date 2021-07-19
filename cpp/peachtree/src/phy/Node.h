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
   	string toSortedNewick(int&, bool printMetaData);
	double getGraphics(bool isRoot, jsonObject& objs, Filtering* filtering, Scaling* scaling, double branchWidth,
			bool showTaxaOnTree, double yshift, double nodeRadius, string internalLabel, string leafLabel, double fontSize, int rounding,
			bool transmissionTree, Timeline* timeline);
	void parseFromNewick(string newick);

	void getAllAnnotations(vector<string>& annots);
	string getAnnotationValue(string var);
	void rotateChildren();
	void addAnnotations(Case* c);
	void cleanup();




private:

	string getNewickMetaData();
	string getTidyMetaData();
	void parseAnnotation(string annotation);

	int nodeNr;
	int filteredNodeNr;
	string acc;
	double height;
	Node* parent;
	vector<Node*> children;
	Taxon* taxon;
	map<string, string> annotations;


protected:
	void setParent(Node* parent);
	void removeParent();


};

#endif /* PHY_NODE_H_ */








