/*
 * Epidemiology.cpp
 *
 *  Created on: 14/07/2021
 *      Author: jdou557
 */

#include "Epidemiology.h"
#include "../Utils.h"
#include "../error/Error.h"


Epidemiology::Epidemiology() {

}


/*
 * Parse a file
 */
void Epidemiology::parseFile(string contents){


	this->cases.clear();
	this->annotations.clear();

	// Split into lines
	contents.erase(std::remove(contents.begin(), contents.end(), '\r'), contents.end());
	vector<string> lines = Utils::split(contents, "\n");

	// Delimiter
	string delimiter = ",";

	// Header
	vector<string> variables;

	// Accession name
	int accessionIndex = -1;


	for (int lineNum = 0; lineNum < lines.size(); lineNum++) {


		string line = lines.at(lineNum);
		Utils::trim(line);

		// Skip comment lines
		if (line.empty()) continue;
		if (line.substr(0, 1) == "#") continue;



		// First line is header
		if (variables.empty()) {

			// What delimiter to use?
			variables = Utils::split(line, delimiter);

			if (variables.size() == 1) {
				delimiter = "\t";
				variables = Utils::split(line, delimiter);
				if (variables.size() == 1) {
					Error::throwError("Please use a comma or tab delimited file with at least 2 columns.");
					return;
				}
			}



			// Ensure there is an accession column
			for (int i = 0; i < variables.size(); i ++) {
				string var = variables.at(i);
				if (accessionIndex == -1 && var == "accession") {
					accessionIndex = i;
				}else {
					if (std::count(this->annotations.begin(), this->annotations.end(), var)) {
						Error::throwError("Duplicate column name " + var);
						return;
					}
					this->annotations.push_back(var);
				}
			}

			if (accessionIndex == -1) {
				Error::throwError("Please ensure the header contains a variable named accession which contains sample names.");
			}



		}


		// Contents line
		else {

			vector<string> lineContents = Utils::split(line, delimiter);

			// Right length?
			if (lineContents.size() != variables.size()) {
				Error::throwError("Line " + to_string(lineNum+1) + " should contain " + to_string(variables.size()) + " elements when split on " + delimiter + "" +
						" but it contains " + to_string(lineContents.size()));
				return;
			}


			// Parse values
			string accession = "";;
			map<string, string> annots;
			for (int i = 0; i < variables.size(); i ++) {
				string var = variables.at(i);
				string val = lineContents.at(i);

				if (i == accessionIndex) {
					accession = val;
				}else {
					annots[var] = val;
				}

			}
			Case* person = new Case(accession, annots);
			this->cases.push_back(person);

		}

	}


	cout << "Successfully parsed " << cases.size() << " cases and " << this->annotations.size() << " annotations" << endl;


}




/*
 * List of column headers / annotations / variable names
 */
vector<string> Epidemiology::getAnnotations(){
	return this->annotations;
}



vector<Case*> Epidemiology::getCases(){
	return this->cases;
}


/*
 * Cleanup
 */
void Epidemiology::cleanup(){
	for (Case* c: cases){
		delete c;
	}
}


/*
 * Validate the accessions in the alignment
 */
void Epidemiology::validateAccessions(Alignment* alignment){


	// Ensure that every sequence is matched by at least one accession
	// Bipartite is not required
	for (string acc1 : alignment->getNames()) {
		bool match = false;
		for (Case* c : this->cases) {
			string acc2 = c->getAccession();
			if (acc1 == acc2) {
				match = true;
				break;
			}
		}


		if (!match) {
			Error::throwError("Cannot find epidemiological annotations for " + acc1);
		}
	}

}


/*
 * Add all annotations to the tree (and check for duplicate variable names)
 */
void Epidemiology::addAnnotationsToTree(Tree* tree){


	// Check for duplicate annotations
	vector<string> treeAnnotations;
	tree->getRoot()->getAllAnnotations(treeAnnotations);
	for (string var : treeAnnotations) {
		if (std::count(this->annotations.begin(), this->annotations.end(), var)) {
			Error::throwError("Duplicate annotation " + var + " found in the tree and the epidemiological data. "
					+ "Please ensure annotation names are unique.");
			return;
		}
	}


	// Annotate each node
	for (Node* node : tree->getNodesAsArray()) {

		// Find the matching case
		Case* c = nullptr;
		for (Case* c2 : this->cases) {
			if (c2->getAccession() == node->getAcc()){
				c = c2;
				break;
			}
		}
		if (c == nullptr) continue;


		node->addAnnotations(c);

	}


}
