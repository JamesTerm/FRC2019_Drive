/****************************** Header ******************************\
Class Name:	ButtonControl inherits ControlItem
File Name:	ButtonControl.cpp
Summary:	Interface for a single button control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson
Email:	dylantrwatson@gmail.com
\*********************************************************************/

#include <iostream>

#include "ButtonControl.h"

using namespace Controls;

ButtonControl::ButtonControl() {}

ButtonControl::ButtonControl(Joystick *_joy, string _name, int _button, bool _actOnRelease, bool _reversed, double _powerMultiplier, bool _isSolenoid)
	: ControlItem(_joy, _name, _reversed, _powerMultiplier) {
	button = _button;
	actOnRelease = _actOnRelease;
	isSolenoid = _isSolenoid;
	previous = 0;
	inc = 0.12;
	isAmpRegulated = false;
	powerPort = 20;
	pdp = new PowerDistributionPanel();
}

double ButtonControl::Update(){
	bool val = joy->GetRawButton(button);
	bool tmp = val;
	//IF IT IS NOT ON A RAMP UP CONTROL
	if(!isRamp && ((!isAmpRegulated) || !(pdp->GetCurrent(powerPort) > ampLimit))){
		if(reversed)
			val = !val;
		current = ((double)val) * powerMultiplier;
		if(components.size() > 0 && !isSolenoid){
			if(val){
				SetToComponents(powerMultiplier);
			}
			else if(actOnRelease && !val){
				SetToComponents(0);
			}
		}
		else if(components.size() > 0 && isSolenoid){
			if(val && reversed){
				SetToSolenoids(DoubleSolenoid::Value::kReverse);
			}
			else if(val && !reversed){
				SetToSolenoids(DoubleSolenoid::Value::kForward);
			}
			else if(!val && actOnRelease){
				SetSolenoidDefault();
			}
		}
		return current;
	}
	else if(isRamp && ((!isAmpRegulated) || !(pdp->GetCurrent(powerPort) > ampLimit))){
		if(components.size() > 0){
			if(val){
				if(abs(abs(previous) - powerMultiplier) >= inc){
					if(getSign(powerMultiplier) == -1)
						current -= inc;
					else if(getSign(powerMultiplier) == 1)
						current += inc;
					SetToComponents(current);
				}
				else if(!(abs(abs(previous) - powerMultiplier) >= inc)){
					current = powerMultiplier;
					SetToComponents(current);
				}
				previous = current;
				return current;
			}
			else if(actOnRelease && !val){
				SetToComponents(0);
				previous = 0;
				current = 0;
				return current;
			}
		}
	}
	else if(isAmpRegulated && (pdp->GetCurrent(powerPort) > ampLimit)){
		if(components.size() > 0){
			if(val){
				double absPWR = abs(previous) - inc;
				if(getSign(powerMultiplier) == -1)
					absPWR *= -1;
				current = absPWR;
				SetToComponents(current);
				previous = current;
				return current;
			}
			else if (actOnRelease && !val){
				SetToComponents(0);
				previous = 0;
				current = 0;
				return current;
			}
		}
	}
	return current;
}

void ButtonControl::SetSolenoidDefault(){
	try
	{
		for(int i=0; i<(int)components.size();i++)
			(*components[i]).DefaultSet();
		}
		catch(...){
			cout << "Error setting default value to binding for " << name << " control!\nYOU MAY HAVE SET A SOLENOID THING TO A MOTOR THING ON BUTTON!" << endl;
		}
}

void ButtonControl::SetToSolenoids(DoubleSolenoid::Value value){
	try{
		for(int i=0; i<(int)components.size();i++)
			(*components[i]).Set(value);
	}
	catch(...){
		cout << "Error setting value to binding for " << name << " control!\nYOU MAY HAVE SET A SOLENOID THING TO A MOTOR THING ON BUTTON!" << endl;
	}
}

void ButtonControl::SetRamp(double _inc){
	if(isSolenoid){
		cout << "WHY DID YOU SET A RAMP TO A SOLENOID" << endl;
		cerr << "WHY DID YOU SET A RAMP TO A SOLENOID" << endl;
		return;
	}
	isRamp = true;
	inc = _inc;
}

void ButtonControl::SetAmpRegulation(int _powerPort, double _ampLimit){
	if(isSolenoid){
		cout << "WHY DID YOU SET AMP REGULATION TO A SOLENOID" << endl;
		cerr << "WHY DID YOU SET AMP REGULATION TO A SOLENOID" << endl;
		return;
	}
	isAmpRegulated = true;
	powerPort = _powerPort;
	ampLimit = _ampLimit;
}

int ButtonControl::getSign(double val){
	if(val < 0)
		return -1;
	else if(val > 0)
		return 1;
	else return 0;
}

ButtonControl::~ButtonControl() {}