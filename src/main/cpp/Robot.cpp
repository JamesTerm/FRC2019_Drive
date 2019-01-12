/****************************** Header ******************************\
Class Name: Robot inherits SampleRobot
File Name: Robot.cpp
Summary: Entry point from WPIlib, main class where robot routines are
started.
Project: BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com
\********************************************************************/

#include <iostream>
#include "Robot.h"
//TODO: Potentiometer include, VictorSPX include
using namespace std;

/**
 * Constructor
 */
Robot::Robot()
{
	m_drive = new Drive();
	m_activeCollection = new ActiveCollection();
}

/**
 * Initialization method of the robot
 * This runs right after Robot() when the code is started
 * Creates Config
 */
void Robot::RobotInit()
{

	cout << "Program Version: " << VERSION << " Revision: " << REVISION << endl;
	//CameraServer::GetInstance()->StartAutomaticCapture();
	Config *config = new Config(m_activeCollection, m_drive); //!< Pointer to the configuration file of the robot
}

/**
 * Method that runs at the beginning of the autonomous mode
 * Uses the SmartDashboard to select the proper Autonomous mode to run
 */
void Robot::Autonomous()
{
	cout << "Autonomous Started." << endl;
	string autoSelected = SmartDashboard::GetString("Auto Selector", m_driveStraight);
	cout << autoSelected << endl;
	//TODO create selector for auton/goal

	//this is to run goals
	double const loopDuration = 0.010;
	double deltaTime = 0;

	Goal* autonGoal = new GoalDoNothing(15.0);
	autonGoal->activate();
	while(IsAutonomous() && !IsDisabled())
	{
		autonGoal->Process(deltaTime);
		Wait(loopDuration);
		deltaTime += loopDuration;
	}
}

/**
 * Called when teleop starts
**/
void Robot::OperatorControl()
{
	cout << "Teleoperation Started." << endl;
	while (IsOperatorControl() && !IsDisabled())
	{
		m_drive->Update();
		Wait(0.010);
	}
}

/**
 * Called when the Test period starts
**/
void Robot::Test() {}

START_ROBOT_CLASS(Robot) //!< This identifies Robot as the main Robot starting class
