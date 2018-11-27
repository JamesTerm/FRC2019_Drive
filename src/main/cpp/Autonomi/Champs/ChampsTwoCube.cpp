/*
 * ChampsTwoCube.cpp
 *
 *  Created on: Apr 19, 2018
 *      Author: Dylann Ruiz
 */
#include <WPILib.h>
#include <fstream>
#include <thread>
#include <iostream>

#include <Autonomi/Champs/ChampsTwoCube.h>
#include <UtilityFunctions.h>
#include <string>
#include <WPILib.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"

using namespace std;
using namespace Autonomi;

ChampsTwoCube::ChampsTwoCube(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

/*
 * This Auto starts from the center and scores two cubes in the switch
 */
void ChampsTwoCube::Start()
{
	gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
	SetArm(.05, activeCollection);

	NavX* navx = activeCollection->GetNavX();
	navx->Reset();

	Wait(.5);
	double straight = navx->GetAngle();

	if(gameData.length() > 0)
	{
		//was 200
		DriveForward(150, .5, activeCollection);

		if(gameData[0] == 'L')
		{
			Turn(-85, activeCollection);

			DriveForward(400, .75,activeCollection);

			Turn(82,activeCollection);

			DriveWithTimer(.5,.5, 1.05, activeCollection);

			if(_IsAutononomous())	DropCube2(activeCollection);

			DriveWithTimer(-.5, -.5, .85, activeCollection);

			TurnShort(65, activeCollection);

			SetArmTimer(-.8, .5, activeCollection);

			SetIntake(1, activeCollection);

			DriveWithTimer(.4, .4, 1.25, activeCollection);
			Wait(.5);

			DriveWithTimer(-.5, -.5, .75, activeCollection);

			SetArmTimer(.5, .85, activeCollection);

			StopIntake(activeCollection);

			TurnNoReset(0, activeCollection);

			DriveWithTimer(.5, .5, 1, activeCollection);

			DropCube(activeCollection);
		}

		else if(gameData[0] == 'R')
		{
			Turn(85,activeCollection);

			DriveForward(150, .75, activeCollection);

			Turn(-85, activeCollection);

			DriveWithTimer(.5,.5, 1.05, activeCollection);

			if(_IsAutononomous())	DropCube2(activeCollection);

			DriveWithTimer(-.5, -.5, .85, activeCollection);

			TurnShort(-65, activeCollection);

			SetArmTimer(-.8, .5, activeCollection);

			SetIntake(1, activeCollection);


			DriveWithTimer(.4, .4, 1.25, activeCollection);
			Wait(.5);

			DriveWithTimer(-.5, -.5, .75, activeCollection);

			SetArmTimer(.5, .85, activeCollection);

			StopIntake(activeCollection);

			TurnNoReset(0, activeCollection);

			DriveWithTimer(.5, .5, 1, activeCollection);

			DropCube(activeCollection);
		}
	}
}
