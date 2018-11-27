/*
 * SwitchLeft.cpp
 *
 *  Created on: Mar 13, 2018
 *      Author: Dylann Ruiz
 */

#include <Autonomi/Sensors/Switch/SwitchLeft.h>
#include <UtilityFunctions.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"
#include <string>
#include <WPILib.h>

using namespace std;
using namespace Autonomi;

SwitchLeft::SwitchLeft(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void SwitchLeft::Start()
{
	gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();

	SetArm(.05, activeCollection);

	NavX* navx = activeCollection->GetNavX();
	navx->Reset();
	double straight = navx->GetAngle();
/*
	if(gameData.length() > 0)
	{

		//5 sec delay for match w 118
		Wait(5.0);
		if(gameData[0] == 'L')
		{
			DriveForward(1300, .50, activeCollection);
			Wait(.25);

			Turn(90, activeCollection);
			Wait(.25);

			straight = navx->GetAngle();
			DriveForward(100, .75, activeCollection);
			Wait(.25);

			if (_IsAutononomous())	DropCube(activeCollection);
		}

		else
		{
			DriveForward(900, .75, activeCollection);
		}


	}*/
	if (gameData[0] == 'L')
	{
		DriveWithTimer(.5,.5,2, activeCollection);
		Turn(90, activeCollection);
		DriveWithTimer(.5,.5, 1, activeCollection);
		DropCube(activeCollection);
	}
	else
	{
		DriveWithTimer(.5,.5,2, activeCollection);

	}

}
