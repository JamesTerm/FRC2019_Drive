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

#include <iostream>

#include "ActiveCollection.h"
#include "../Systems/Drive.h"
#include "../Controls/ButtonControl.h"
#include "../Controls/AxisControl.h"
#include "../Controls/ToggleButtonControl.h"
#include "../Pugi/pugixml.h"

using namespace System;
using namespace pugi;
namespace Configuration{

class Config {
public:
	Config(ActiveCollection *_activeCollection, Drive *_drive);
	//TODO: Make this a bool return
	void LoadValues(xml_document &doc);
	void AllocateComponents(xml_node &root);
	void AllocateDriverControls(xml_node &root);
	void AllocateComponentsDep();
	virtual ~Config();
private:
	Joystick *m_driveJoy;
	Joystick *m_operatorJoy;
	ActiveCollection *m_activeCollection;
	Drive *m_drive;
	bool getAttribute(xml_node &node, const char_t* attr);
};

}
#endif /* SRC_CONFIG_CONFIG_H_ */
