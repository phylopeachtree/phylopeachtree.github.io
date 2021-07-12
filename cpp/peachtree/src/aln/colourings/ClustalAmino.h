/*
 * ClustalAmino.h
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */
#include "Colouring.h"


#ifndef ALN_COLOURINGS_CLUSTALAMINO_H_
#define ALN_COLOURINGS_CLUSTALAMINO_H_

class ClustalAmino : public Colouring {
public:
	ClustalAmino();
	virtual string getName() override;
	virtual bool isNucleotide() override;

};

#endif /* ALN_COLOURINGS_CLUSTALAMINO_H_ */
