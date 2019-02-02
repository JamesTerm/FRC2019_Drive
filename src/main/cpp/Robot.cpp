/****************************** Header ******************************\
Class Name: Robot inherits SampleRobot
File Name: Robot.cpp
Summary: Entry point from WPIlib, main class where robot routines are
started.
Project: BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson, Chris Weeks
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com, 
chrisrweeks@aol.com
\********************************************************************/

#include <iostream>
#include "Robot.h"
//TODO: Potentiometer include, VictorSPX include
using namespace std;

/*
 * Constructor
 */
Robot::Robot()
{
	m_drive = new Drive();
	m_activeCollection = new ActiveCollection();
}

/*
 * Initialization method of the robot
 * This runs right after Robot() when the code is started
 * Creates Config
 */
void Robot::RobotInit()
{

	cout << "Program Version: " << VERSION << " Revision: " << REVISION << endl;
	CameraServer::GetInstance()->StartAutomaticCapture();
	Config *config = new Config(m_activeCollection, m_drive); //!< Pointer to the configuration file of the robot
	m_inst = nt::NetworkTableInstance::GetDefault(); //!Network tables
	m_visionTable = m_inst.GetTable("VISION_2019"); //!Vision network table

}

/*
 * Method that runs at the beginning of the autonomous mode
 * Uses the SmartDashboard to select the proper Autonomous mode to run
  TODO: Integrate with our dashboad; Integrate with TeleOp for sandstorm; make everything that should be abstract is in fact abstract; Integrate vision overlay and drive cam to dashboard; Write teleop goals;
 */

void Robot::Autonomous()
{
	m_masterGoal = new MultitaskGoal(m_activeCollection, false);

	cout << "Autonomous Started." << endl;
	string autoSelected = SmartDashboard::GetString("Auto Selector", m_driveStraight);
	cout << autoSelected << endl;
	SelectAuton(m_activeCollection, m_masterGoal, autoSelected);
	m_masterGoal->AddGoal(new Goal_TimeOut(m_activeCollection, 15.0));
	//m_masterGoal->AddGoal(new Goal_WaitThenDrive(m_activeCollection, .5, .5, 3, 5));
	m_masterGoal->Activate();
	double dTime = 0.010;
	while (m_masterGoal->GetStatus() == Goal::eActive)
	{
		m_masterGoal->Process(dTime);
		Wait(dTime);
	}
	m_masterGoal->~MultitaskGoal();
	cout << "goal loop complete" << endl;
	cout << m_activeCollection->GetNavX()->GetAngle() << endl;
#if 0 //old code
		if (autoSelected == m_driveStraight) //!< Drive Straight Autonomous
		{
			DriveStraight *driveStraight = new DriveStraight(m_activeCollection);
			driveStraight->Start();
		}

		else //!< Default Autonomous
		{
			DriveStraight *driveStraight = new DriveStraight(m_activeCollection);
			driveStraight->Start();
		}
#endif
}

/*
 * Called when teleop starts
 */
void Robot::OperatorControl()
{
	cout << "Teleoperation Started." << endl;
	while (IsOperatorControl() && !IsDisabled())
	{
		cout << "Teleoperation Started." << endl;
		while (IsOperatorControl() && !IsDisabled())
		{
			m_drive->Update();
		}
	}
}

/*
 * Called when the Test period starts
 */
void Robot::Test() {}
START_ROBOT_CLASS(Robot) //!< This identifies Robot as the main Robot starting class
