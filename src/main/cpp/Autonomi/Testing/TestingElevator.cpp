/*
 * Dylann.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#include <string>
#include <WPILib.h>
#include <Autonomi/Testing/TestingElevator.h>
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
using namespace Components;

TestingElevator::TestingElevator(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void TestingElevator::Start()
{
	//Turn(-70, straight,activeCollection)
	TalonSRXItem *lift = activeCollection->GetTalon("lift");
	double gane = 4.0;
	double targetPos = 750;
	while(lift->GetQuadraturePosition() < targetPos && _IsAutononomous())
	{
		double currentEncVal = lift->GetQuadraturePosition() * 1.0;
		double err = (targetPos - currentEncVal)/targetPos;
		double setPower(err * gane);
	}
	lift->Set(0);
}

