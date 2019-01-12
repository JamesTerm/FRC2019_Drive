/****************************** Header ******************************\
Class Name:	ToggleButtonControl inherits ControlItem
File Name:	ToggleButtonControl.cpp
Summary:	Interface for a toggle button control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobot983@gmail.com
\*********************************************************************/

#include "ToggleButtonControl.h"

using namespace Controls;

ToggleButtonControl::ToggleButtonControl() { }

ToggleButtonControl::ToggleButtonControl(Joystick *_joy, string _name, int _button, bool _IsSwitchOnPress, bool _IsReversed, double _powerMultiplier, bool _IsSolenoid): ControlItem(_joy, _name, _IsReversed, _powerMultiplier)
{
	joy = _joy;
	name = _name;
	button = _button;
	IsSwitchOnPress = _IsSwitchOnPress;
	IsSolenoid = _IsSolenoid;
	if(IsSwitchOnPress)
	{
		State = false;
		LastState = false;
	}
	else
	{
		State = true;
		LastState = true;
	}
}

//Ian notes:
//SetToComponents()-motor
//SetSolenoids() - direction
//SolenDefalt
double ToggleButtonControl::Update()
{

	bool Val = joy->GetRawButton(button);

	current = ((double)Val) * powerMultiplier;
	//Switching
	if(Val && IsSwitchOnPress && !State && (LastState != Val))
	{
		State = true;
		LastState = true;
	}
	else if(!Val && IsSwitchOnPress && State && (LastState != Val))
	{
		State = false;
		LastState = false;
	}
	//What the button will do now

	if(IsSolenoid)
	{
		if(State){
			
		}
		else{
			SetSolenoidDefalt();
		}
	}

	return current;
}

void ToggleButtonControl::SetSolenoids(DoubleSolenoid::Value value)
{
	for(int i=0; i<(int)components.size();i++)
	{
		(*components[i]).Set(value);
	}
}

void ToggleButtonControl::SetSolenoidDefalt()
{
	for(int i=0; i<(int)components.size();i++)
	{
		((DoubleSolenoidItem*)(components[i]))->DefaultSet();
	}
}

ToggleButtonControl::~ToggleButtonControl() { }

