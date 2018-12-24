/****************************** Header ******************************\
Class Name: Config
File Name:	Config.h
Summary: 	Manages and loads the configuration file.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com
\********************************************************************/


#ifndef SRC_CONFIG_CONFIG_H_
#define SRC_CONFIG_CONFIG_H_

#include "ActiveCollection.h"
#include "../Systems/SystemsCollection.h"
#include "../Controls/ButtonControl.h"
#include "../Controls/AxisControl.h"

using namespace Systems;
namespace Configuration{

class Config {
public:
	Joystick *driveJoy;
	Joystick *operatorJoy;
	ActiveCollection *activeCollection;
	SystemsCollection systemsCollection;
	Config(ActiveCollection *_activeCollection);
	//TODO: Make this a bool return
	void AllocateComponents();
	virtual ~Config();
};

}
#endif /* SRC_CONFIG_CONFIG_H_ */
