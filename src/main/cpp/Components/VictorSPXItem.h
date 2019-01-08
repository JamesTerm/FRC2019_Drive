/****************************** Header ******************************\
Class Name: VictorSPXItem inherits OutputComponent
File Name:	VictorSPXItem.h
Summary: Abstraction for the VictorSPX
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot983@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_TALONSPXITEM_H_
#define SRC_COMPONENTS_TALONSPXITEM_H_

#include "ctre/Phoenix.h"
#include "OutputComponent.h"

using namespace std;
using namespace frc;

namespace Components{
class VictorSPXItem : public OutputComponent{
private:
	VictorSPX *victor;
	int channel;
	bool reversed;

public:
	VictorSPXItem();
	VictorSPXItem(int channel, string name, bool reversed);
	virtual double Get() override;
	virtual void Set(double val) override;
	virtual void Set(DoubleSolenoid::Value value) override;
	virtual void DefaultSet() override;
	virtual ~VictorSPXItem();
};
}

#endif /* SRC_COMPONENTS_TALONSPXITEM_H_ */