/****************************** Header ******************************\
Class Name: Robot inherits SampleRobot
File Name: Robot.cpp
Summary: Entry point from WPIlib, main class where robot routines are
started.
Project: BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson
Email: cooper.ryan@centaurisoftware.co, dylan.watson@broncbotz.org
\********************************************************************/

#include <WPILib.h>
#include <fstream>
#include <thread>
#include <iostream>
#include <string>

#include "Autonomi/Autons.h"
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Systems/Initializers.h"
#include "Systems/SystemsCollection.h"

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
	std::thread *drive; //!< Pointer to where we will allocate the drive task.

public:

	ActiveCollection *activeCollection; //!< Pointer to the only instantiation of the ActiveCollection Class in the program

	/**
	 * Constructor
	 */
	Robot() { }

	/**
	 * Initialization method of the robot
	 * This runs right after Robot() when the code is started
	 * Creates Config
	 */
	
	void RobotInit() override
	{
		
		activeCollection = new ActiveCollection();
		Config *config = new Config(activeCollection); //!< Pointer to the configuration file of the robot
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
//		DRIVE STRAIGHT

		if (autoSelected == drivestraight)
		{
			DriveStraight *driveStraight = new DriveStraight(activeCollection);
			driveStraight->Start();
		}


//		TIMERS
		else if (autoSelected == tSwCenter)
		{
			TimerSwitchCenter *test = new TimerSwitchCenter(activeCollection);
			test->Start();
		}


//		SWITCH
		else if (autoSelected == swLeft)
		{
			SwitchLeft *switchLeft = new SwitchLeft(activeCollection);
			switchLeft->Start();
		}
		else if (autoSelected == swRight)
		{
			SwitchRight *switchRight = new SwitchRight(activeCollection);
			switchRight->Start();
		}
		/*else if (autoSelected == swCenter)
		{
			SwitchCenter* test = new SwitchCenter(activeCollection);
			test->Start();
		}*/


//		SCALE
		else if (autoSelected == scLeft)
		{
			ScaleLeft *test  = new ScaleLeft(activeCollection);
			test->Start();
		}
		else if(autoSelected == scRight)
		{
			ScaleRight *test = new ScaleRight(activeCollection);
			test->Start();
		}


//		NO CROSS
/*		else if (autoSelected == swScNCLeft)
		{
			SwitchScaleNoCrossLeft *test = new SwitchScaleNoCrossLeft(activeCollection);
			test->Start();
		}
		else if (autoSelected == swScNCRight)
		{
			SwitchScaleNoCrossRight* test = new SwitchScaleNoCrossRight(activeCollection);
			test->Start();
		}

//		ELIMS EL PASO
		else if (autoSelected == elimsSwL)
		{
			ElimsSwitchLeft * test = new ElimsSwitchLeft(activeCollection);
			test->Start();
		}
		else if (autoSelected == elimsSwR)
		{
			ElimsSwitchRight * test = new ElimsSwitchRight(activeCollection);
			test->Start();
		}*/

//		TESTING

		else if (autoSelected == dylann)
		{
			Dylann *dylann = new Dylann(activeCollection);
			dylann->Start();
		}
		else if (autoSelected == testNavX)
		{
			TestNavX *test  = new TestNavX(activeCollection);
			test->Start();
		}
		else if (autoSelected == testEncoders)
		{
			TestEncoders *test  = new TestEncoders(activeCollection);
			test->Start();
		}
		else if (autoSelected == pid)
		{
			RealNibbaShit *test = new RealNibbaShit(activeCollection);
			test->Start();
		}


//		CHAMPS
		else if (autoSelected == twocube)
		{
			ChampsTwoCube *test = new ChampsTwoCube(activeCollection);
			test->Start();
		}/*
		else if (autoSelected == cLeft)
		{
			ChampsLeft *test = new ChampsLeft(activeCollection);
			test->Start();
		}
		else if (autoSelected == cRight)
		{
			ChampsRight *test = new ChampsRight(activeCollection);
			test->Start();
		}
*/

//		DEFAULT
		else
		{
			DriveStraight *driveStraight = new DriveStraight(activeCollection);
			driveStraight->Start();
		}
	}

    /**
    * Called when teleop starts
    **/
   
	void OperatorControl() override
	{
		cout << "OPERATOR REACHED" << endl;
		InitializeDrive();
	}

	void Test() override {}

private:
	/** AUTON NAME DEFINITIONS **/
	//TODO: Make this into it's own file

	const string drivestraight = "drive";

	const string tSwCenter = "SwitchCenter";

	const string swLeft = "SwitchLeft";
	const string swRight = "SwitchRight";
	//const string swCenter = "SwitchCenter";

	const string scLeft = "ScaleLeft";
	const string scRight = "ScaleRight";

	const string swScNCLeft = "NoCrossLeft";
	const string swScNCRight = "NoCrossRight";

	const string elimsSwL = "EPElimsSwitchLeft";
	const string elimsSwR = "EPElimsSwitchRight";

	const string dylann = "Dylann";
	const string testNavX = "navx";
	const string testEncoders = "encoders";
	const string pid = "pid";

	const string twocube = "TwoCube";
	const string cLeft = "ChampsLeft";
	const string cRight = "ChampsRight";
};

START_ROBOT_CLASS(Robot) //!< This identifies Robot as the main Robot starting class
