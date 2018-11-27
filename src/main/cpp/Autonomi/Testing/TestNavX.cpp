/*
 * Dylann.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#include <string>
#include <WPILib.h>
#include <Autonomi/Testing/TestNavX.h>
#include <UtilityFunctions.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"
#include "Components/EncoderItem.h"
#include <vector>
#include "Util/LoopChecks.h"

using namespace std;
using namespace Autonomi;
using namespace Configuration;

TestNavX::TestNavX(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void TestNavX::Start()
{
	activeCollection->GetNavX()->Reset();
	Wait(.5);
	double current = activeCollection->GetNavX()->GetAngle();

	cout << "initial= " << current << endl;
	while (_IsAutononomous())
	{
		current = activeCollection->GetNavX()->GetAngle();

		Wait(.05);
		cout << "angle:: " << current << endl;
	}

	cout << "final= " << current << endl;
}

