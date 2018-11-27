/****************************** Header ******************************\
Class Name: -
File Name: 	Initializers.h
Summary: 	Collection of methods to specifically call the initializers
		 	for each congruent loop
Project:    BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ryan Cooper
Email:	cooper.ryan@centaurisoft.org
\*********************************************************************/
#ifndef INCLUDE_INITIALIZERS_H_
#define INCLUDE_INITIALIZERS_H_

#include <WPILib.h>

#include "SystemsCollection.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

namespace Systems{
	void InitializeDrive(){ SystemsCollection::Instance().drive->Initialize(); }//!< Indirectly creates an instance of the drive and calls it's Initialize function.
}

#pragma GCC diagnostic pop

#endif /* INCLUDE_INITIALIZERS_H_ */
