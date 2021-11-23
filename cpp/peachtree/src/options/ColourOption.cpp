/*
 * ColourOption.cpp
 *
 *  Created on: 9/08/2021
 *      Author: jdou557
 */

#include "ColourOption.h"
#include "../Utils.h"


ColourOption::ColourOption(string name, string section, string title){
	this->name = name;
	this->section = section;
	this->title = title;
	this->longTitle = title;
	this->hidden = false;

	initCols();
	this->setVal(this->colours.at(0));
}


ColourOption::ColourOption(string name, string section, string title, string value){
	this->name = name;
	this->section = section;
	this->title = title;
	this->longTitle = title;
	this->hidden = false;

	initCols();
	this->setVal(value);
}


ColourOption::ColourOption(string name, string section, string title, string value, bool hidden){
	this->name = name;
	this->section = section;
	this->title = title;
	this->hidden = hidden;

	initCols();
	this->setVal(value);
}



/*
 * Get the palette of the currently selected colour
 */
vector<string> ColourOption::getPalette(){
	return this->colourPalettes.at(this->currentIndex);
}


void ColourOption::initCols(){
	
	// https://colorbrewer2.org/
	this->colourPalettes.clear();
	vector<int> colourIndices;
	
	
	// Black to white
	this->colourPalettes.push_back(vector<string> { "#ffffff","#f0f0f0","#d9d9d9","#bdbdbd","#969696","#737373","#525252","#252525","#000000" });
	colourIndices.push_back(8);
	
	// Grey to red
	this->colourPalettes.push_back(vector<string> { "#b2182b","#d6604d","#f4a582","#fddbc7","#ffffff","#e0e0e0","#bababa","#878787","#4d4d4d" });
	colourIndices.push_back(8);

	// Brown to yellow
	this->colourPalettes.push_back(vector<string> { "#ffffe5","#fff7bc","#fee391","#fec44f","#fe9929","#ec7014","#cc4c02","#993404","#662506" });
	colourIndices.push_back(7);
	
	

	// Red to yellow
	this->colourPalettes.push_back(vector<string> { "#ffffcc","#ffeda0","#fed976","#feb24c","#fd8d3c","#fc4e2a","#e31a1c","#bd0026","#800026" });
	colourIndices.push_back(6);
	
	// Maroon to white
	this->colourPalettes.push_back(vector<string> { "#f7f4f9","#e7e1ef","#d4b9da","#c994c7","#df65b0","#e7298a","#ce1256","#980043","#67001f" });
	colourIndices.push_back(6);
	
	
	// Green to red
	this->colourPalettes.push_back(vector<string> { "#d73027","#f46d43","#fdae61","#fee08b","#ffffbf","#d9ef8b","#a6d96a","#66bd63","#1a9850" });
	colourIndices.push_back(1);
	
	// Purple to orange
	this->colourPalettes.push_back(vector<string> { "#b35806","#e08214","#fdb863","#fee0b6","#f7f7f7","#d8daeb","#b2abd2","#8073ac","#542788" });
	colourIndices.push_back(1);
	
	// Rainbow
	this->colourPalettes.push_back(vector<string> { "#e41a1c","#377eb8","#4daf4a","#984ea3","#ff7f00","#ffff33","#a65628","#f781bf","#999999" });
	colourIndices.push_back(5);
	
	
	// Green to pink
	this->colourPalettes.push_back(vector<string> { "#c51b7d","#de77ae","#f1b6da","#fde0ef","#f7f7f7","#e6f5d0","#b8e186","#7fbc41","#4d9221" });
	colourIndices.push_back(8);

	// Green to white
	this->colourPalettes.push_back(vector<string> { "#f7fcf5","#e5f5e0","#c7e9c0","#a1d99b","#74c476","#41ab5d","#238b45","#006d2c","#00441b" });
	colourIndices.push_back(5);
	
	// Green to yellow
	this->colourPalettes.push_back(vector<string> { "#ffffe5","#f7fcb9","#d9f0a3","#addd8e","#78c679","#41ab5d","#238443","#006837","#004529" });
	colourIndices.push_back(7);
	

	// Deep blue to brown
	this->colourPalettes.push_back(vector<string> { "#8c510a","#bf812d","#dfc27d","#f6e8c3","#f5f5f5","#c7eae5","#80cdc1","#35978f","#01665e" });
	colourIndices.push_back(8);



	// Blue to yellow
	this->colourPalettes.push_back(vector<string> { "#ffffd9","#edf8b1","#c7e9b4","#7fcdbb","#41b6c4","#1d91c0","#225ea8","#253494","#081d58" });
	colourIndices.push_back(5);
	
	
	// Blue to red
	this->colourPalettes.push_back(vector<string> { "#b2182b","#d6604d","#f4a582","#fddbc7","#f7f7f7","#d1e5f0","#92c5de","#4393c3","#2166ac" });
	colourIndices.push_back(8);
	
	
	// Blue to white
	this->colourPalettes.push_back(vector<string> { "#fff7fb","#ece7f2","#d0d1e6","#a6bddb","#74a9cf","#3690c0","#0570b0","#045a8d","#023858" });
	colourIndices.push_back(7);



	// Purple to pink
	this->colourPalettes.push_back(vector<string> { "#fff7f3","#fde0dd","#fcc5c0","#fa9fb5","#f768a1","#dd3497","#ae017e","#7a0177","#49006a" });
	colourIndices.push_back(5);
	

	// Purple to white
	this->colourPalettes.push_back(vector<string> { "#fcfbfd","#efedf5","#dadaeb","#bcbddc","#9e9ac8","#807dba","#6a51a3","#54278f","#3f007d" });
	colourIndices.push_back(6);
	
	
	// Green to purple
	this->colourPalettes.push_back(vector<string> { "#762a83","#9970ab","#c2a5cf","#e7d4e8","#f7f7f7","#d9f0d3","#a6dba0","#5aae61","#1b7837" });
	colourIndices.push_back(0);
	
	


	// Set colour list to the specified element in each palette
	for (int i = 0; i < this->colourPalettes.size(); i++){
		vector<string> palette = this->colourPalettes.at(i);
		int pos = colourIndices.at(i);
		string col = palette.at(pos);
		this->colours.push_back(col);
		
	}
	
	
	
	/*
	this->colours.push_back("#242424"); // dark grey
	this->colours.push_back("#444444"); // dark/med grey
	this->colours.push_back("#696969"); // med grey
	this->colours.push_back("#999999"); // med/light grey
	this->colours.push_back("#d3d3d3"); // light grey


	this->colours.push_back("#ba2e00"); // dark red
	this->colours.push_back("#ff0b08"); // red
	this->colours.push_back("#ff8608"); // orange
	this->colours.push_back("#fdff08"); // yellow
	this->colours.push_back("#8bbb00"); // light green
	this->colours.push_back("#00ba2e"); // green
	this->colours.push_back("#008cba"); // blue
	this->colours.push_back("#002ebb"); // dark blue
	this->colours.push_back("#29465b"); // dark blue grey
	this->colours.push_back("#FFDAB9"); // peach
	this->colours.push_back("#ba008c"); // magenta
	this->colours.push_back("#8e00bb"); // purple
	*/

}


jsonObject ColourOption::toJSON() {
	jsonObject json;
	jsonObject arr;
	json["value"] = this->value;
	json["value"] = this->value;
	json["type"] = "ColourOption";
	for (int i = 0; i < this->colours.size(); i ++){
		string col = this->colours.at(i);
		arr.push_back(col);
	}
	json["colours"] = arr;

	json = modifyJSON(json);
	return json;

}



string ColourOption::getVal() {
	return this->value;
}

void ColourOption::setVal(string value) {
	value = Utils::toLower(value);
	
	this->currentIndex = 0;
	for (int i = 0; i < this->colours.size(); i ++){
		string col = Utils::toLower(this->colours.at(i));
		if (col == value){
			this->currentIndex = i;
			this->value = value;
			return;
		}
	}
	// this->value = this->colours.at(0);
}



