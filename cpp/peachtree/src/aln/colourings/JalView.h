/*
 * JalView.h
 *
 *  Created on: 8/07/2021
 *      Author: jdou557
 */
#include "Colouring.h"


#ifndef ALN_COLOURINGS_JALVIEW_H_
#define ALN_COLOURINGS_JALVIEW_H_

class JalView : public Colouring {
public:
	JalView();
	virtual string getName() override;
	virtual bool isNucleotide() override;

};

#endif /* ALN_COLOURINGS_JALVIEW_H_ */
