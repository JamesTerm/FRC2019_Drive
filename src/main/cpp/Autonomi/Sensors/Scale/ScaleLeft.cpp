/*
 * ScaleLeft.cpp
 *
 *  Created on: Mar 27, 2018
 *      Author: Dylann Ruiz
 */

#include <string>
#include <WPILib.h>
#include <Autonomi/Sensors/Scale/ScaleLeft.h>
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

ScaleLeft::ScaleLeft(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void ScaleLeft::Start()
{
	gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
	SetArm(.05, activeCollection);

	NavX* navx = activeCollection->GetNavX();
	navx->Reset();
	double straight = navx->GetAngle();

	if(gameData.length() > 0)
	{
		if(gameData[1] == 'L')
		{
			DriveLong(1700, .55, activeCollection);

			TurnShort(17, activeCollection);

			SetArm(-.5, activeCollection);
			Wait(.2);

			StopArm(activeCollection);

			SetElevator(.9, activeCollection);
			Wait(1.5);
			StopElevator(activeCollection);

			DriveLong(750, .35, activeCollection);

			SetIntake(.23, activeCollection);
			Wait(.5);
			StopIntake(activeCollection);

			DriveWithTimer(-.25, -.25, 1, activeCollection);

			SetArm(-.5, activeCollection);
			Wait(.2);

			StopArm(activeCollection);

			SetElevator(-.75, activeCollection);
			Wait(1);

			StopElevator(activeCollection);

			//cout << "final angle = " << navx->GetAngle() << endl;
		}

		else
		{
			//was 2000
			DriveLong(1400, .5, activeCollection);

			Turn(90,activeCollection);

			DriveLong(1000, .5,activeCollection);

			Turn(-90, activeCollection);

			SetArm(-.5, activeCollection);
			Wait(.2);

			StopArm(activeCollection);

			SetElevator(.90, activeCollection);
			Wait(1.5);
			StopElevator(activeCollection);

			DriveWithTimer(.25, .25, .5, activeCollection);

			SetIntake(.23, activeCollection);
			Wait(.5);
			StopIntake(activeCollection);

			SetDrive(-.25, -.25, activeCollection);
			SetElevator(-.90, activeCollection);
			Wait(1.25);

			StopDrive(activeCollection);
			StopElevator(activeCollection);

			TurnLong(165, activeCollection);
		}
	}
}

