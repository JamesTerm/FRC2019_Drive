/*
 * ScaleRight.cpp
 *
 *  Created on: Mar 27, 2018
 *      Author: Dylann Ruiz
 */

#include <string>
#include <WPILib.h>
#include <Autonomi/Sensors/Scale/ScaleRight.h>
#include <UtilityFunctions.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"
#include "Components/EncoderItem.h"
#include "Components/TalonSRXItem.h"
#include <vector>
#include "Util/LoopChecks.h"

using namespace std;
using namespace Autonomi;

ScaleRight::ScaleRight(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void ScaleRight::Start()
{

	gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
	SetArm(.40, activeCollection);


	NavX* navx = activeCollection->GetNavX();
	navx->Reset();
	double straight = navx->GetAngle();

	if(gameData.length() > 0)
	{
		if(gameData[1] == 'R')
		{
			DriveForward(2000, .75, activeCollection);

			SetArm(-.5, activeCollection);
			Wait(.25);
			SetElevator(1, activeCollection);
			Wait(.65);

			StopArm(activeCollection);

			Wait(.35);

			StopElevator(activeCollection);

			DriveWithTimer(.25, .25, .5, activeCollection);

			SetIntake(.3, activeCollection);
			Wait(.5);

			DriveWithTimer(-.25, -.25, 2, activeCollection);

			StopElevator(activeCollection);
			StopIntake(activeCollection);
		}

		else if (gameData[1] == 'L')
		{
			DriveForward(2400, .5, activeCollection);

			Turn(-90,activeCollection);

			DriveForward(1000, .5,activeCollection);

			Turn(90, activeCollection);

			SetArm(-.5, activeCollection);
			Wait(.25);
			SetElevator(1, activeCollection);
			Wait(.65);

			StopArm(activeCollection);

			Wait(.35);

			StopElevator(activeCollection);

			DriveWithTimer(.25, .25, .5, activeCollection);

			SetIntake(.3, activeCollection);
			Wait(1);

			DriveWithTimer(-.25, -.25, 1, activeCollection);

			StopElevator(activeCollection);
			StopIntake(activeCollection);
		}
	}
}
