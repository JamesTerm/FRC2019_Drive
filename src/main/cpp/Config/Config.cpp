
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
#include "..\Systems\SystemsCollection.h"

using namespace std;
using namespace Systems;
using namespace Configuration;

Config::Config(ActiveCollection *_activeCollection) {
	driveJoy = new Joystick(0);
	operatorJoy = new Joystick(1);
	activeCollection = _activeCollection;
	AllocateComponents();
	cout << "CONFIG CONSTRUCTOR COMPLETE" << endl;
}

void Config::AllocateComponents(){
	cout << "ALLOCATE COMPONENTS" << endl;
	EncoderItem *enc0 = new EncoderItem("enc0", 2, 3, false);
	activeCollection->Add(enc0);

	//EncoderItem *enc1 = new EncoderItem("enc1", 2, 3, false);
	//activeCollection->Add(enc1);

/********************** NAVX DEFINITIONS **************************/

/*	NavX *navX = new NavX();
	activeCollection->Add(navX);
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

	TalonSRXItem *lift = new TalonSRXItem(24, "Lift", true);

	VictorSPItem *leftIntake = new VictorSPItem("LeftIntake", 6, false);
	VictorSPItem *rightIntake = new VictorSPItem("RightIntake", 7, false);

	VictorSPItem *intakeDrop = new VictorSPItem("IntakeDrop", 8, false);

	DigitalInputItem *liftSwitch = new DigitalInputItem(4, "InputTest");

/***********************  SOLENOID DEFINITIONS  *******************/
cout << "COMPONENT DEFINITIONS COMPLETED" << endl;
/*********************** ACTIVECOLLECTION CALLS *******************/

	activeCollection->Add(left_0);
	activeCollection->Add(left_1);
	activeCollection->Add(left_2);
	activeCollection->Add(right_0);
	activeCollection->Add(right_1);
	activeCollection->Add(right_2);

	activeCollection->Add(climb_0);
	activeCollection->Add(climb_1);
	activeCollection->Add(lift);
	activeCollection->Add(leftIntake);
	activeCollection->Add(rightIntake);
	activeCollection->Add(intakeDrop);

	activeCollection->Add(liftSwitch);
cout << "ACTIVE COLLECTION ADDS COMPLETED" << endl;
/*********************** DRIVE CONTROL DEFINITIONS ******************/

	AxisControl *leftDrive = new AxisControl(driveJoy, "LeftDrive", 1, 0.07, true, 0.70);
	AxisControl *rightDrive = new AxisControl(driveJoy, "RightDrive", 5, 0.07, true, 0.70);
cout << "AXIS CONTROL COMPLETED" << endl;
/*********************** DRIVE ADDITIONS ***************************/

	drive->AddControlDrive(leftDrive);
cout << "LEFT DRIVE" << endl;
	drive->AddControlDrive(rightDrive);
cout << "DRIVE ADDITIONS COMPLETED" << endl;
/*********************** DRIVE BINDINGS ****************************/
	leftDrive->AddComponent(left_0);
	leftDrive->AddComponent(left_1);
	leftDrive->AddComponent(left_2);
cout << "DRIVE BINDINGS START" << endl;
	rightDrive->AddComponent(right_0);
	rightDrive->AddComponent(right_1);
	rightDrive->AddComponent(right_2);
cout << "DRIVE BINDINGS COMPLETE" << endl;
/********************* OPERATE CONTROL DEFINITIONS ******************/

	AxisControl *intakeDropControl = new AxisControl(operatorJoy, "IntakeDropControl", 5, 0.3, true, 0.50);
	AxisControl *liftControl = new AxisControl(operatorJoy, "LiftControl", 1, 0.1, true, .9);
	AxisControl *climbControl = new AxisControl(operatorJoy, "ClimbContol", 2, 0.07, false, 0.9);
	AxisControl *intakeOutFastControl = new AxisControl(operatorJoy, "IntakeOutSlow", 3, .07, false, -.6);
	ButtonControl *intakeOutSlowControl = new ButtonControl(operatorJoy, "IntakeOutSlow", 2, false, false, -.3, false);
	ButtonControl *intakeInControlRight = new ButtonControl(operatorJoy, "IntakeIn", 3, true, false, .75, false);
	ButtonControl *intakeInControlLeft = new ButtonControl(operatorJoy, "IntakeIn", 3, true, false, .9, false);
//	ButtonControl *intakeInControlLeftBump = new ButtonControl(operatorJoy, "IntakeIn", 5, false, false, .5, false);
//	ButtonControl *intakeInControlRightBump = new ButtonControl(operatorJoy, "IntakeIn", 6, false, false, .5, false);

/*********************** SYSTEMS CONTROL OPERATOR *******************/

	drive->AddControlOperate(intakeDropControl);
	drive->AddControlOperate(liftControl);
	drive->AddControlOperate(intakeInControlRight);
	drive->AddControlOperate(intakeInControlLeft);
//	systemsCollection.drive->AddControlOperate(intakeInControlLeftBump);
//	systemsCollection.drive->AddControlOperate(intakeInControlRightBump);
	drive->AddControlOperate(intakeOutSlowControl);
	drive->AddControlOperate(climbControl);
	drive->AddControlOperate(intakeOutFastControl);

/********************** OPERATOR BINDINGS **************************/

	intakeDropControl->AddComponent(intakeDrop);

	liftControl->AddComponent(lift);

	intakeInControlRight->AddComponent(rightIntake);
	intakeInControlLeft->AddComponent(leftIntake);
//	intakeInControlLeftBump->AddComponent(leftIntake);
//	intakeInControlRightBump->AddComponent(rightIntake);

	intakeOutSlowControl->AddComponent(rightIntake);
	intakeOutSlowControl->AddComponent(leftIntake);

	intakeOutFastControl->AddComponent(rightIntake);
	intakeOutFastControl->AddComponent(leftIntake);

	climbControl->AddComponent(climb_0);
	climbControl->AddComponent(climb_1);

	intakeInControlRight->SetRamp(0.1);
	intakeInControlLeft->SetRamp(0.1);
//	intakeInControlLeftBump->SetRamp(0.1);
//	intakeInControlRightBump->SetRamp(0.1);

	liftControl->SetLift(liftSwitch, 3.0);

/************ SYSTEMS COLLECTION ACTIVE COLLECTION ****************/

	drive->activeCollection = activeCollection;

}

Config::~Config(){

}
