/*
 * NodeX.h
 *
 *  Created on: 13/07/2021
 *      Author: jdou557
 */

#ifndef PHY_NODEX_H_
#define PHY_NODEX_H_


#include <iostream>
#include <vector>

#include "Node.h"

using namespace std;

class NodeX {
public:
	NodeX(vector<string> taxaNames);

	void m_iLeftInstance(int val);
	int m_iLeftInstance();
	void m_iRightInstance(int val);
	int m_iRightInstance();
	void m_left(NodeX* node);
	NodeX* m_left();
	void m_right(NodeX* node);
	NodeX* m_right();
	void m_parent(NodeX* node);
	NodeX* m_parent();
	void setHeight(double height1, double height2);
	void setLength(double length1, double length2);
	double m_fHeight();
	string toString();
	Node* toNode();

private:
	const double EPSILON = 0; //1e-10;
	vector<string> taxaNames;

	NodeX* left;
	NodeX* right;
	NodeX* parent;
	int iLeftInstance;
	int iRightInstance;
	double fLeftLength;
	double fRightLength;
	double fHeight;




};

#endif /* PHY_NODEX_H_ */
