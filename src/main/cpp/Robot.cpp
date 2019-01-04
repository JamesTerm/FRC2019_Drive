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

#include <WPILib.h>
#include <fstream>
#include <thread>
#include <iostream>
#include <string>

#include "Autonomi/Autons.h"
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Systems/Drive.h"

using namespace frc;
using namespace std;
using namespace Autonomi;
using namespace Configuration;
using namespace Systems;

#define VERSION 1 //!< Defines the program version for the entire program.
#define REVISION "A" //!< Defines the revision of this version of the program.

/**
 * Main entry point of the program, inherits SampleRobot (WPILib) to control the robot
 */
class Robot : public SampleRobot
{

public:

	/**
	 * Constructor
	 */
	Robot() {
		cout << "CONSTRUCTOR 21" << endl;
	 }

	/**
	 * Initialization method of the robot
	 * This runs right after Robot() when the code is started
	 * Creates Config
	 */
	
	void RobotInit() override
	{
		cout << "ROBOT INIT" << endl;
		m_drive = new Drive();
		m_activeCollection = new ActiveCollection();
		cout << "ACTIVE COLLECTION COMPLETED" << endl;
		Config *config = new Config(m_activeCollection, m_drive); //!< Pointer to the configuration file of the robot
		cout << "CONFIG COMPLETED" << endl;
		cout << "Program Version: " << VERSION << " Revision: " << REVISION << endl;
	}

	/**
	 * Method that runs at the beginning of the autonomous mode
	 * Uses the SmartDashboard to select the proper Autonomous mode to run
	 */
	void Autonomous() override
	{
		
		string autoSelected = SmartDashboard::GetString("Auto Selector", drivestraight);
		cout << autoSelected << endl;

		if (autoSelected == drivestraight) //!< Drive Straight Autonomous
		{
			DriveStraight *driveStraight = new DriveStraight(m_activeCollection);
			driveStraight->Start();
		}

		else //!< Default Autonomous
		{
			DriveStraight *driveStraight = new DriveStraight(m_activeCollection);
			driveStraight->Start();
		}
	}

    /**
    * Called when teleop starts
    **/
	void OperatorControl() override
	{
		cout << "OPERATOR REACHED" << endl;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		m_drive->Initialize();
#pragma GCC diagnostic pop
	}

	/**
	 * Called when the Test period starts
	**/
	void Test() override {}

private:
	/** 
	 * Autonomous name definitions 
	**/

	//TODO: Make this into it's own file

	ActiveCollection *m_activeCollection; //!< Pointer to the only instantiation of the ActiveCollection Class in the program
	Drive *m_drive; //!< Pointer to the only instantiation of the Drive class in the program
	const string drivestraight = "drive";
};

START_ROBOT_CLASS(Robot) //!< This identifies Robot as the main Robot starting class
