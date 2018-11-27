/****************************** Header ******************************\
Class Name:	ToggleButtonControl inherits ControlItem
File Name:	ToggleButtonControl.cpp
Summary:	Interface for a toggle button control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson
Email:	dylan.watson@broncbotz.org
\*********************************************************************/

#include <Controls/ToggleButtonControl.h>

using namespace Controls;

ToggleButtonControl::ToggleButtonControl() {}

ToggleButtonControl::ToggleButtonControl(Joystick *_joy, string _name, int _button){
	joy = _joy;
	name = _name;
	button = _button;
}

double ToggleButtonControl::Update(){
return 0.0;
}

ToggleButtonControl::~ToggleButtonControl() {}

