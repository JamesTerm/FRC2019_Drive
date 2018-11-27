/*
 * Dylann.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#include <string>
#include <WPILib.h>
#include <Autonomi/Testing/TestEncoders.h>
#include <UtilityFunctions.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"
#include "Components/EncoderItem.h"
#include <vector>
#include "Util/LoopChecks.h"

using namespace std;
using namespace Autonomi;
using namespace Components;

TestEncoders::TestEncoders(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void TestEncoders::Start()
{
	EncoderItem *enc0 = activeCollection->GetEncoder("enc0");
	enc0->Reset();

	double enc;

	while (_IsAutononomous())
	{
		enc = enc0->Get();
		cout << "encoder value = " << enc << endl;
		Wait(.05);
	}
}

