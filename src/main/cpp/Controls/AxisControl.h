/****************************** Header ******************************\
Class Name:	AxisControl inherits ControlItem
File Name:	AxisControl.h
Summary:	Interface for an axis control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson
Email:	dylan.watson@broncbotz.org
\*********************************************************************/


#ifndef SRC_CONTROLS_AXISCONTROL_H_
#define SRC_CONTROLS_AXISCONTROL_H_

#include <string>

#include "WPILib.h"
#include "ControlItem.h"
#include "Util/Constants.h"
#include "Components/DigitalInputItem.h"

using namespace std;

namespace Controls{
class AxisControl : public ControlItem{
private:
	int axis;
	double deadZone;
	int getSign(double val);
	int getSignIntake(double val);
	double targetAmp;
	double currentAmp;
	double currentPow;
	int channel;
	double err;
	double gane;
	bool isIntakeDrop;
	int previousSign;
	bool isLift;
	bool isIdle;
	double targetEncVal;
	int encoderResetPos = 50000;
	DigitalInputItem *limit;
	PowerDistributionPanel *pdb = new PowerDistributionPanel();

public:
	AxisControl();
	AxisControl(Joystick *_joy, string _name, int _axis, double _deadZone, bool _reversed, double _powerMultiplier);
	void SetLift(DigitalInputItem* _limit, double _gane);
	void SetIntakeDrop();
	virtual double Update() override;
	virtual ~AxisControl();
};
}

#endif /* SRC_CONTROLS_AXISCONTROL_H_ */
