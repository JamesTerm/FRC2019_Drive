/*
 * SwitchCenter.cpp
 *
 *  Created on: Apr 30, 2018
 *      Author: Dylann Ruiz
 */

#include <Autonomi/Sensors/Switch/SwitchCenter.h>
#include <UtilityFunctions.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"
#include <string>
#include <WPILib.h>

using namespace std;
using namespace Autonomi;

SwitchCenter::SwitchCenter(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void SwitchCenter::Start()
{

	gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
	SetArm(.05, activeCollection);

	NavX* navx = activeCollection->GetNavX();
	navx->Reset();
	Wait(.5);
	double straight = navx->GetAngle();

	if(gameData.length() > 0)
	{
		DriveForward(200, .5, activeCollection);

		if(gameData[0] == 'L')
		{
			Turn(-85, activeCollection);

			DriveForward(375, .6,activeCollection);

			Turn(85,activeCollection);

			DriveWithTimer(.5,.5, 1, activeCollection);

			if(_IsAutononomous())	DropCube(activeCollection);
		}

		else if(gameData[0] == 'R')
		{
			Turn(85,activeCollection);

			DriveForward(300, .6, activeCollection);

			Turn(-85, activeCollection);

			DriveWithTimer(.5,.5, 1, activeCollection);

			if(_IsAutononomous())	DropCube(activeCollection);
		}
	}

} /* namespace SwitchCenter */
