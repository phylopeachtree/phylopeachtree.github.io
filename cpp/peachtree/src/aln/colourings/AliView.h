/*
 * AliView.h
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */
#include "Colouring.h"


#ifndef ALN_COLOURINGS_ALIVIEW_H_
#define ALN_COLOURINGS_ALIVIEW_H_

class AliView : public Colouring {
public:
	AliView();
	virtual string getName() override;
	virtual bool isNucleotide() override;

};

#endif /* ALN_COLOURINGS_ALIVIEW_H_ */
