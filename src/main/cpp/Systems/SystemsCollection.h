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
#include "Singleton.h"

namespace Systems
{

/*! SystemsCollection is a singleton that stores the active instances of all required independently run systems.*/
class SystemsCollection final : public Singleton<SystemsCollection>
{
	friend class Singleton<SystemsCollection>;
public:
	SystemsCollection(){}//!< Default constructor.
	~SystemsCollection(){ } //!< Destructor.

	Drive *drive = new Drive(); //!< Creates and instance of the Drive system.


	//testing to try and fix twitch bug, maybe these still run because we are using a singleton?
	void Reset()
	{
		delete drive;
		drive = new Drive();
	}
};

} /* namespace Systems */

#endif /* SYSTEMS_SYSTEMSCOLLECTION_H_ */
