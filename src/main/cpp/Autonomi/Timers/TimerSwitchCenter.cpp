	/*
 * ScoreCube.cpp
 *
 *  Created on: Mar 4, 2018
 *      Author: Dylann Ruiz
 */
#include <WPILib.h>
#include <fstream>
#include <thread>
#include <iostream>

#include <Autonomi/Timers/TimerSwitchCenter.h>
#include <UtilityFunctions.h>
#include <string>
#include <WPILib.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"

using namespace std;
using namespace Autonomi;

TimerSwitchCenter::TimerSwitchCenter(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void TimerSwitchCenter::Start()
{
	gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
	SetArm(.15, activeCollection);

	if (gameData.length() > 0)
	{
		DriveWithTimer(.25+error, .25, 1.6, activeCollection);

		if (gameData[0] == 'L')
		{
			DriveWithTimer(-.25-error, .25, .6, activeCollection);

			DriveWithTimer(.25+error, .25, 1.6, activeCollection);

			DriveWithTimer(.25+error, -.25, .5, activeCollection);

			DriveWithTimer(.25+error, .25, 2.15, activeCollection);

			DropCube(activeCollection);
		}

		else if (gameData[0] == 'R')
		{
			DriveWithTimer(.25+error, -.25, .7, activeCollection);

			DriveWithTimer(.25+error, .25, 1.5, activeCollection);

			DriveWithTimer(-.25-error, .25, .75, activeCollection);

			DriveWithTimer(.25+error, .25, 2.15, activeCollection);

			DropCube(activeCollection);
		}
		else
		{
			DriveWithTimer(.25+error, .25, 3, activeCollection);
		}
	}
}

