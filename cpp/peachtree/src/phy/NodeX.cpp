/*
 * NodeX.cpp
 *
 *  Created on: 13/07/2021
 *      Author: jdou557
 */

#include "NodeX.h"



NodeX::NodeX(vector<string> taxaNames) {
	this->taxaNames = taxaNames;
	this->left = nullptr;
	this->right = nullptr;
	this->parent = nullptr;
	this->fLeftLength = 0;
    this->fRightLength = 0;
    this->fHeight = 0;
    this->iLeftInstance = 0;
    this->iRightInstance = 0;
}


void NodeX::m_iLeftInstance(int val){
	this->iLeftInstance = val;
}

int NodeX::m_iLeftInstance(){
	return this->iLeftInstance;
}

void NodeX::m_iRightInstance(int val){
	this->iRightInstance = val;
}

int NodeX::m_iRightInstance(){
	return this->iRightInstance;
}

void NodeX::m_left(NodeX* node){
	this->left = node;
}

NodeX* NodeX::m_left(){
	return this->left;
}

void NodeX::m_right(NodeX* node){
	this->right = node;
}

NodeX* NodeX::m_right(){
	return this->right;
}

void NodeX::m_parent(NodeX* node){
	this->parent = node;
}

NodeX* NodeX::m_parent(){
	return this->parent;
}

double NodeX::m_fHeight(){
	return this->fHeight;
}

void NodeX::setHeight(double height1, double height2){
	if (height1 < EPSILON) {
		height1 = EPSILON;
	}
	if (height2 < EPSILON) {
		height2 = EPSILON;
	}
	fHeight = height1;
	if (left == nullptr) {
		fLeftLength = height1;
	} else {
		fLeftLength = height1 - left->m_fHeight();
	}
	if (right == nullptr) {
		fRightLength = height2;
	} else {
		fRightLength = height2 - right->m_fHeight();
	}
}

void NodeX::setLength(double length1, double length2){
	 if (length1 < EPSILON) {
			length1 = EPSILON;
		}
		if (length2 < EPSILON) {
			length2 = EPSILON;
		}
		fLeftLength = length1;
		fRightLength = length2;
		fHeight = length1;
		if (left != nullptr) {
			fHeight += left->m_fHeight();
		}
}

string NodeX::toString(){
	return "tmp";
}

Node* NodeX::toNode(){


    Node* node = new Node();
    node->setHeight(fHeight);

    if (left == nullptr) {

    	Node* left = new Node();

    	left->setNr(iLeftInstance);
    	left->setHeight(fHeight - fLeftLength);
    	left->setAcc(taxaNames.at(iLeftInstance));
    	node->addChild(left);
        if (right == nullptr) {
        	Node* right = new Node();
            right->setNr(iRightInstance);
            right->setHeight(fHeight - fRightLength);
            right->setAcc(taxaNames.at(iRightInstance));
            node->addChild(right);
        } else {
            node->addChild(right->toNode());
        }
    } else {
        node->addChild(left->toNode());
        if (right == nullptr) {
        	Node* right = new Node();
            node->addChild(right);
            right->setNr(iRightInstance);
            right->setHeight(fHeight - fRightLength);
            right->setAcc(taxaNames.at(iRightInstance));
        } else {
        	node->addChild(right->toNode());
        }
    }
    if (node->getHeight() < node->getChild(0)->getHeight() + EPSILON) {
        node->setHeight(node->getChild(0)->getHeight() + EPSILON);
    }
    if (node->getHeight() < node->getChild(1)->getHeight() + EPSILON) {
        node->setHeight(node->getChild(1)->getHeight() + EPSILON);
    }


    return node;
}

