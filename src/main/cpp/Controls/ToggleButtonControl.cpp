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

using namespace std;
using namespace Controls;

ToggleButtonControl::ToggleButtonControl() { }

ToggleButtonControl::ToggleButtonControl(Joystick *_joy, string _name, int _button, bool _IsSwitchOnPress, bool _IsReversed, double _powerMultiplier, bool _IsSolenoid): ControlItem(_joy, _name, _IsReversed, _powerMultiplier)
{
	joy = _joy;
	name = _name;
	button = _button;
	IsSwitchOnPress = _IsSwitchOnPress;
	IsSolenoid = _IsSolenoid;
	IsReversed = _IsReversed;
	State = false;
	out = false;
	LastState = false;
}

double ToggleButtonControl::Update()
{

	bool Val = joy->GetRawButton(button);

	current = ((double)State) * powerMultiplier;

	//Switching for state to be used
	if(IsSwitchOnPress){
		State = ButtonDown(Val);
	}
	else{
		State = ButtonUp(Val);
	}
	
	if(IsSolenoid)
	{
		if(State){
			//gets default value for solenoid to be used to move the other direction
			DoubleSolenoid::Value def = GetSolenoidDefaultValue();
			if(def == DoubleSolenoid::Value::kForward){
				SetSolenoids(DoubleSolenoid::Value::kReverse);
			}
			else if (def == DoubleSolenoid::Value::kReverse){
				SetSolenoids(DoubleSolenoid::Value::kForward);
			}
			else{
				cout << "The defalt of the solenoid is off" << endl;
			}

		}
		else{
			SetSolenoidDefalt();
		}
	}
	else if(!IsSolenoid)
	{
		if(State)
		{
			//sets the motor current
			if(components.size() > 0){
				if(IsReversed){
					SetToComponents(-current);
				}
				else if(!IsReversed){
					SetToComponents(current);
				}
			}
			else{
				cout << "No Components found" << endl;
			}
		}
		else{
			SetToComponents(0);
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

bool ToggleButtonControl::ButtonDown(bool input)
{
	//switches state when the button is pressed
	if(input){
		if(!LastState){
			out = !out;
			LastState = true;

			cout << "Flip state" << endl;
		}
	}
	if(!input){
		LastState = false;
	}

	return out;
}

bool ToggleButtonControl::ButtonUp(bool input)
{
	//switches state when the button is released
	if(!input){
		if(LastState){
			out = !out;
			LastState = false;

			cout << "Flip state" << endl;
		}
	}
	if(input){
		LastState = true;
	}

	return out;
}

DoubleSolenoid::Value ToggleButtonControl::GetSolenoidValue()
{
	DoubleSolenoid::Value ret = DoubleSolenoid::kOff;
	for(int i=0; i<(int)components.size();i++)
	{
		//TODO: Fix for MultiComponent Use
		ret=((DoubleSolenoidItem*)(components[i]))->GetState();
		
	}
	return ret;
}

DoubleSolenoid::Value ToggleButtonControl::GetSolenoidDefaultValue()
{
	DoubleSolenoid::Value ret = DoubleSolenoid::kOff;
	for(int i=0; i<(int)components.size();i++)
	{
		//TODO: Fix for MultiComponent Use
		ret=((DoubleSolenoidItem*)(components[i]))->GetDefaultValue();
		
	}
	return ret;
}

ToggleButtonControl::~ToggleButtonControl() { }

