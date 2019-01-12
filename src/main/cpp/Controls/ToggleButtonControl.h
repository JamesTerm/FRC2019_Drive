/****************************** Header ******************************\
Class Name:	ToggleButtonControl inherits ControlItem
File Name:	ToggleButtonControl.h
Summary:	Interface for a toggle button control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobbot983@gmail.com
\*********************************************************************/

#ifndef SRC_CONTROLS_TOGGLEBUTTONCONTROL_H_
#define SRC_CONTROLS_TOGGLEBUTTONCONTROL_H_

#include <iostream>
#include "ControlItem.h"
#include "../Components/DoubleSolenoidItem.h"
#include "../Components/VictorSPItem.h"

using namespace frc;

namespace Controls
{
	class ToggleButtonControl : public ControlItem
	{
		private:
			Joystick *joy;
			string name;
			int button;
			bool State;
			bool IsSwitchOnPress;
			bool LastState;
			double current;
			bool IsSolenoid;
			bool IsReversed;

		public:
			ToggleButtonControl();
			ToggleButtonControl(Joystick *_joy, string _name, int _button, bool _IsSwitchOnPress, bool _IsReversed, double _powerMultiplier, bool _IsSolenoid);
			void SetSolenoids(DoubleSolenoid::Value value);
			void SetSolenoidDefalt();
			DoubleSolenoid::Value GetSolenoidValue();
			virtual ~ToggleButtonControl();
			virtual double Update() override;
	};
}

#endif /* SRC_CONTROLS_TOGGLEBUTTONCONTROL_H_ */
