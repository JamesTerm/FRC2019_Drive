/****************************** Header ******************************\
Class Name:	ToggleButtonControl inherits ControlItem
File Name:	ToggleButtonControl.h
Summary:	Interface for a toggle button control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson
Email:	dylantrwatson@gmail.com
\*********************************************************************/

#ifndef SRC_CONTROLS_TOGGLEBUTTONCONTROL_H_
#define SRC_CONTROLS_TOGGLEBUTTONCONTROL_H_

#include "ControlItem.h"

using namespace frc;
//TODO: actually write this
namespace Controls{
class ToggleButtonControl : public ControlItem{
private:
	Joystick *joy;
	string name;
	int button;

public:
	ToggleButtonControl();
	ToggleButtonControl(Joystick *_joy, string _name, int _button);
	virtual ~ToggleButtonControl();
	virtual double Update() override;
};
}

#endif /* SRC_CONTROLS_TOGGLEBUTTONCONTROL_H_ */
