/****************************** Header ******************************\
Class Name: Config
File Name:	Config.h
Summary: 	Manages and loads the configuration file.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com
\********************************************************************/

#include "Config.h"

using namespace std;
using namespace System;
using namespace Configuration;

Config::Config(ActiveCollection *_activeCollection, Drive *_drive) {
	m_driveJoy = new Joystick(0);
	m_operatorJoy = new Joystick(1);
	m_activeCollection = _activeCollection;
	m_drive = _drive;
	AllocateComponents();
}

void Config::AllocateComponents(){

	VictorSPItem *left_0 = new VictorSPItem("Left_0", 2, true);
	VictorSPItem *left_1 = new VictorSPItem("Left_1", 3, true);
	VictorSPItem *right_0 = new VictorSPItem("Right_0", 0, false);
	VictorSPItem *right_1 = new VictorSPItem("Right_1", 1, false);
	DoubleSolenoidItem *SolenoidTest = new DoubleSolenoidItem("SolenoidToggle", 1, 0, DoubleSolenoid::Value::kReverse, false);

	m_activeCollection->Add(left_0);
	m_activeCollection->Add(left_1);
	m_activeCollection->Add(right_0);
	m_activeCollection->Add(right_1);
	m_activeCollection->Add(SolenoidTest);

	NavX *navx = new NavX();
	m_activeCollection->Add(navx);

	AxisControl *leftDrive = new AxisControl(m_driveJoy, "LeftDrive", 1, 0.07, true, 0.70);
	AxisControl *rightDrive = new AxisControl(m_driveJoy, "RightDrive", 5, 0.07, true, 0.70);
	ToggleButtonControl *ToggleTest = new ToggleButtonControl(m_driveJoy, "ToggleTest", 3, true, false, 0.3, true);

	m_drive->AddControlDrive(leftDrive);
	m_drive->AddControlDrive(rightDrive);
	m_drive->AddControlDrive(ToggleTest);


	leftDrive->AddComponent(left_0);
	leftDrive->AddComponent(left_1);
	rightDrive->AddComponent(right_0);
	rightDrive->AddComponent(right_1);
	ToggleTest->AddComponent(SolenoidTest);
	

}

Config::~Config(){

}
