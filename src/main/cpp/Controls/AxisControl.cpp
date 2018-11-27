/****************************** Header ******************************\
Class Name:	AxisControl inherits ControlItem
File Name:	AxisControl.cpp
Summary:	Interface for an axis control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson
Email:	dylan.watson@broncbotz.org
\*********************************************************************/


#include <cmath>

#include <Controls/AxisControl.h>
#include "ControlItem.h"
#include "Components/DigitalInputItem.h"
#include "Components/TalonSRXItem.h"
#include "Util/Constants.h"
#include <WPILib.h>

bool stopped;

using namespace Controls;
using namespace Util;

AxisControl::AxisControl(){}

AxisControl::AxisControl(Joystick *_joy, string _name, int _axis, double _deadZone, bool _reversed, double _powerMultiplier)
	: ControlItem(_joy, _name, _reversed, _powerMultiplier){
	axis = _axis;
	deadZone = _deadZone;
	isIntakeDrop = false;
	previousSign = 0;
	isLift = false;
}

double AxisControl::Update(){
	double raw = (*joy).GetRawAxis(axis);
	//cout << "RAW: " << raw << endl;
	bool limitDepressed;
	if(isLift){
		limitDepressed = limit->GetBool();
	}
	else{
		limitDepressed = false;
	}
	double dz = deadZone + Constants::Instance().MINIMUM_JOYSTICK_RETURN;
	if(name == "LiftControl"){
		//cout << "LIMIT VAL: " << limit->GetBool() << endl;
		//cout << "RAW: " << raw << endl;
	}
	if(!(abs(raw) > deadZone)){
		if(previousSign == -1 && isIntakeDrop){
			SetToComponents(-0.15);
			currentPow = -0.15;
			return currentPow;
		}
		else if(previousSign == 1 && isIntakeDrop){
			SetToComponents(0.15);
			currentPow = 0.15;
			return currentPow;
		}
		if(currentPow != 0 && !isLift){
			SetToComponents(0);
			currentPow = 0;
		}
		else if(isLift && !limitDepressed){
			double currentEncVal = ((TalonSRXItem*)components[0])->GetQuadraturePosition() * 1.0;
			//cout << "CURRENT ENC VAL: " << currentEncVal << endl;
			if(!isIdle){
				targetEncVal = currentEncVal;
				isIdle = true;
				return currentPow;
			}
			//cout << "TARGET VAL: " << targetEncVal << endl;
			err = (targetEncVal - currentEncVal)/targetEncVal;
			//cout << "ERR: " << err << endl;
			currentPow = err * gane;
			SetToComponents(currentPow);
			return currentPow;
		}
		else if(isLift && limitDepressed){
			//cout << "limit hit" << endl;
			isIdle = false;
			((TalonSRXItem*)components[0])->SetQuadraturePosition(encoderResetPos);
			targetEncVal = encoderResetPos;
			SetToComponents(0);
			currentPow = 0;
		}
		return 0;
	}
	double val = ((abs(raw) - dz) * (pow(1-dz, -1)) * getSign(raw)) * powerMultiplier;
	if(reversed)
		val = -val;
	SetToComponents(val);
	isIdle = false;
	currentPow = val;
	previousSign = getSign(val);
	return val;
}

int AxisControl::getSign(double val){
	if(val < 0)
		return -1;
	else
		return 1;
}

int AxisControl::getSignIntake(double val){
	if(val < 0)
		return -1;
	else if(val > 0)
		return 1;
	else return 0;
}

void AxisControl::SetLift(DigitalInputItem* _limit, double _gane){
	if(isIntakeDrop){
		//cout << "CAN'T SET AXIS " << name << " TO THE LIFT, IT IS THE INTAKE DROP!" << endl;
		return;
	}else if(components.size() != 1){
		//cout << "CAN'T SET AXIS " << name << " TO THE LIFT, IT HAS " << components.size() << " BINDINGS WHEN IT SHOULD HAVE ONE!" << endl;
		return;
	}
	isLift = true;
	gane = _gane;
	limit = _limit;
}

void AxisControl::SetIntakeDrop(){
	if(isLift){
		//cout << "CAN'T SET AXIS " << name << " TO THE INTAKE DROP, IT IS THE LIFT!" << endl;
		return;
	}
	isIntakeDrop = true;
}

AxisControl::~AxisControl() {}

