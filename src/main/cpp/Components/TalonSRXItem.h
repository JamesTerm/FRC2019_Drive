/****************************** Header ******************************\
Class Name: TalonSRXItem inherits OutputComponent
File Name:	TalonSRXItem.h
Summary: Abstraction for the WPIlib TalonSRX that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_TALONSRXITEM_H_
#define SRC_COMPONENTS_TALONSRXITEM_H_

#include <ctre/Phoenix.h>
#include "OutputComponent.h"

using namespace std;
using namespace frc;

namespace Components{
class TalonSRXItem : public OutputComponent{
private:
	TalonSRX *talon;
	int channel;
	bool reversed;

public:
	TalonSRXItem();
	TalonSRXItem(int channel, string name, bool reversed);
	int GetQuadraturePosition();
	void SetQuadraturePosition(int val);
	virtual double Get() override;
	virtual void Set(double val) override;
	virtual void Set(DoubleSolenoid::Value value) override;
	virtual void DefaultSet() override;
	virtual ~TalonSRXItem();
};
}

#endif /* SRC_COMPONENTS_TALONSRXITEM_H_ */
