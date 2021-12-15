/*
 * AlphaAmino.h
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */
#include "Colouring.h"


#ifndef ALN_COLOURINGS_ALPHAAMINO_H_
#define ALN_COLOURINGS_ALPHAAMINO_H_

class AlphaAmino : public Colouring {
public:
	AlphaAmino();
	virtual string getName() override;
	virtual bool isNucleotide() override;
	virtual bool isAlpha() override;
};

#endif /* ALN_COLOURINGS_ALPHAAMINO_H_ */
