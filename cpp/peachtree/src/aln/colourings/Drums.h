/*
 * Drums.h
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */
#include "Colouring.h"


#ifndef ALN_COLOURINGS_DRUMS_H_
#define ALN_COLOURINGS_DRUMS_H_

class Drums : public Colouring {
public:
	Drums();
	virtual string getName() override;
	virtual bool isNucleotide() override;

};

#endif /* ALN_COLOURINGS_DRUMS_H_ */
