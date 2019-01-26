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
		m_masterGoal = new MultitaskGoal(m_activeCollection, false);
		cout << "Autonomous Started." << endl;
				string autoSelected = SmartDashboard::GetString("Auto Selector", m_driveStraight);
		cout << autoSelected << endl;

		//m_masterGoal->AddGoal(new Goal_DriveWithTimer(m_activeCollection, .5, .5, 5.0));
		//m_masterGoal->AddGoal(new Goal_DriveWithTimer(m_activeCollection,.5,.5,10.0));
		m_masterGoal->AddGoal(new Goal_WaitThenDrive(m_activeCollection,.2,.2,5.0,5.0));
		//m_masterGoal->AddGoal(new Goal_Timer(m_activeCollection,15.0));
		cout << "test1" << endl;
		m_masterGoal->Activate();
		//cout << m_masterGoal->GetStatus() << endl;
		//Pcout << m_masterGoal->listSize() << endl;
		double dTime = 0.010;
		while(m_masterGoal->GetStatus() == Goal::eActive)
		{
			//cout << "loop" << endl;
			m_masterGoal->Process(dTime);
			Wait(dTime);
		}
		m_masterGoal->~MultitaskGoal();
		cout << "goal loop complete" << endl;
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
