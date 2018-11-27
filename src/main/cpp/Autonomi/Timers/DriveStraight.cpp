
/*
 * Dylann.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#include <WPILib.h>
#include <fstream>
#include <thread>
#include <iostream>

#include <Autonomi/Timers/DriveStraight.h>
#include <UtilityFunctions.h>
#include "Config/ActiveCollection.h"
#include "Config/Config.h"
#include "Components/VictorSPItem.h"
#include <string>
#include <WPILib.h>

using namespace std;
using namespace Autonomi;

DriveStraight::DriveStraight(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void DriveStraight::Start()
{
	cout << "drive straight" << endl;
	//DriveForward(500, .75, activeCollection);
	DriveWithTimer(.5,.5,2, activeCollection);
}

