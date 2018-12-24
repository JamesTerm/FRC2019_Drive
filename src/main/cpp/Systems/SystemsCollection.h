/****************************** Header ******************************\
Class Name:  SystemsCollection inherits Singleton
File Name: SystemsCollection.h
Summary: 	 Holds and instance of each major system, this is a singleton
			 as we [should] only have one instance of each system, I cannot
			 think of any scenario where another instance should be created.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ryan Cooper
Email:	cooper.ryan@centaurisoftware.co
\*********************************************************************/
#ifndef SYSTEMS_SYSTEMSCOLLECTION_H_
#define SYSTEMS_SYSTEMSCOLLECTION_H_

#include "Drive.h"

namespace Systems
{
 	static Drive *drive;

	static void Initialize_Systems()
	{
		drive = new Drive();
	}
} /* namespace Systems */

#endif /* SYSTEMS_SYSTEMSCOLLECTION_H_ */
