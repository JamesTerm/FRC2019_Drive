/****************************** Header ******************************\
Class Name: VictorSPXItem inherits OutputComponent
File Name:	VictorSPXItem.cpp
Summary: Abstraction for the VictorSPX
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot983@gmail.com
\********************************************************************/

#include <iostream>

#include "VictorSPXItem.h"

using namespace std;
using namespace frc;
using namespace Components;

VictorSPXItem::VictorSPXItem() {}

VictorSPXItem::VictorSPXItem(int _channel, string _name, bool _reversed)
	: OutputComponent(_name){
	channel = _channel;
	reversed = _reversed;
	Victor = new VictorSPX(channel);
	Victor->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 10);
}

double VictorSPXItem::Get(){
    if (reversed)
        return Victor->GetMotorOutputPercent() * -1;
    return Victor->GetMotorOutputPercent();
}

int VictorSPXItem::GetQuadraturePosition(){
	return Victor->GetSensorCollection().GetQuadraturePosition();
}

void VictorSPXItem::SetQuadraturePosition(int val){
	Victor->GetSensorCollection().SetQuadraturePosition(val, 10);
}

void VictorSPXItem::Set(double val){
	if((val<0 || val>0) && !inUse)
	{
		inUse = true;
		if(reversed) Victor->Set(ControlMode::PercentOutput, -val);
		else Victor->Set(ControlMode::PercentOutput, val);
		inUse = false;
	}
	else if(!inUse){
		inUse = true;
		Victor->Set(ControlMode::PercentOutput, 0);
		inUse = false;
	}
}

void VictorSPXItem::DefaultSet(){
	cout << "WHY DID YOU CALL THE DEFAULT SET FOR A MOTOR RETARD -Watson" << endl;
}

void VictorSPXItem::Set(DoubleSolenoid::Value value){
	cout << "WHY DID YOU CALL THE SOLENOID SET FOR A MOTOR RETARD -Watson" << endl;
}

VictorSPXItem::~VictorSPXItem() {}
