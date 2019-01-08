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
//	EncoderItem *enc0 = new EncoderItem("enc0", 2, 3, false);
//	m_activeCollection->Add(enc0);

	//EncoderItem *enc1 = new EncoderItem("enc1", 2, 3, false);
	//m_activeCollection->Add(enc1);

/********************** NAVX DEFINITIONS **************************/

/*	NavX *navX = new NavX();
	m_activeCollection->Add(navX);
*/
/********************** MOTOR DEFINITIONS *************************/

	VictorSPItem *left_0 = new VictorSPItem("Left_0", 0, false);
	VictorSPItem *left_1 = new VictorSPItem("Left_1", 1, false);
	VictorSPItem *left_2 = new VictorSPItem("Left_2", 2, false);

	VictorSPItem *right_0 = new VictorSPItem("Right_0", 3, true);
	VictorSPItem *right_1 = new VictorSPItem("Right_1", 4, true);
	VictorSPItem *right_2 = new VictorSPItem("Right_2", 5, true);

	VictorSPItem *climb_0 = new VictorSPItem("Climb_0", 9, true);
	VictorSPItem *climb_1 = new VictorSPItem("Climb_1", 17, true);
	//TalonSRXItem *climb_1 = new TalonSRXItem(0,"Climb_1", true);

//	TalonSRXItem *lift = new TalonSRXItem(24, "Lift", true);

	VictorSPItem *leftIntake = new VictorSPItem("LeftIntake", 6, false);
	VictorSPItem *rightIntake = new VictorSPItem("RightIntake", 7, false);

	VictorSPItem *intakeDrop = new VictorSPItem("IntakeDrop", 8, false);

//	DigitalInputItem *liftSwitch = new DigitalInputItem(4, "InputTest");

/***********************  SOLENOID DEFINITIONS  *******************/
/*********************** m_activeCollection CALLS *******************/

	m_activeCollection->Add(left_0);
	m_activeCollection->Add(left_1);
	m_activeCollection->Add(left_2);
	m_activeCollection->Add(right_0);
	m_activeCollection->Add(right_1);
	m_activeCollection->Add(right_2);

	m_activeCollection->Add(climb_0);
	m_activeCollection->Add(climb_1);
//	m_activeCollection->Add(lift);
	m_activeCollection->Add(leftIntake);
	m_activeCollection->Add(rightIntake);
	m_activeCollection->Add(intakeDrop);

//	m_activeCollection->Add(liftSwitch);
/*********************** DRIVE CONTROL DEFINITIONS ******************/

	AxisControl *leftDrive = new AxisControl(m_driveJoy, "LeftDrive", 1, 0.07, true, 0.70);
	AxisControl *rightDrive = new AxisControl(m_driveJoy, "RightDrive", 5, 0.07, true, 0.70);
/*********************** DRIVE ADDITIONS ***************************/

	m_drive->AddControlDrive(leftDrive);
	m_drive->AddControlDrive(rightDrive);
/*********************** DRIVE BINDINGS ****************************/
	leftDrive->AddComponent(left_0);
	leftDrive->AddComponent(left_1);
	leftDrive->AddComponent(left_2);
	rightDrive->AddComponent(right_0);
	rightDrive->AddComponent(right_1);
	rightDrive->AddComponent(right_2);
/********************* OPERATE CONTROL DEFINITIONS ******************/

	AxisControl *intakeDropControl = new AxisControl(m_operatorJoy, "IntakeDropControl", 5, 0.3, true, 0.50);
	AxisControl *liftControl = new AxisControl(m_operatorJoy, "LiftControl", 1, 0.1, true, .9);
	AxisControl *climbControl = new AxisControl(m_operatorJoy, "ClimbContol", 2, 0.07, false, 0.9);
	AxisControl *intakeOutFastControl = new AxisControl(m_operatorJoy, "IntakeOutSlow", 3, .07, false, -.6);
	ButtonControl *intakeOutSlowControl = new ButtonControl(m_operatorJoy, "IntakeOutSlow", 2, false, false, -.3, false);
	ButtonControl *intakeInControlRight = new ButtonControl(m_operatorJoy, "IntakeIn", 3, true, false, .75, false);
	ButtonControl *intakeInControlLeft = new ButtonControl(m_operatorJoy, "IntakeIn", 3, true, false, .9, false);

/*********************** SYSTEMS CONTROL OPERATOR *******************/

	m_drive->AddControlOperate(intakeDropControl);
	m_drive->AddControlOperate(liftControl);
	m_drive->AddControlOperate(intakeInControlRight);
	m_drive->AddControlOperate(intakeInControlLeft);
	m_drive->AddControlOperate(intakeOutSlowControl);
	m_drive->AddControlOperate(climbControl);
	m_drive->AddControlOperate(intakeOutFastControl);

/********************** OPERATOR BINDINGS **************************/

	intakeDropControl->AddComponent(intakeDrop);

//	liftControl->AddComponent(lift);

	intakeInControlRight->AddComponent(rightIntake);
	intakeInControlLeft->AddComponent(leftIntake);

	intakeOutSlowControl->AddComponent(rightIntake);
	intakeOutSlowControl->AddComponent(leftIntake);

	intakeOutFastControl->AddComponent(rightIntake);
	intakeOutFastControl->AddComponent(leftIntake);

	climbControl->AddComponent(climb_0);
	climbControl->AddComponent(climb_1);

	intakeInControlRight->SetRamp(0.1);
	intakeInControlLeft->SetRamp(0.1);

//	liftControl->SetLift(liftSwitch, 3.0);
}

Config::~Config(){

}
