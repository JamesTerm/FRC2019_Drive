/****************************** Header ******************************\
Class Name:	Drive
File Name:	Drive.cpp
Summary: 	Drive loop
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ryan Cooper, Dylan Watson
Email:	cooper.ryan@centaurisoft.org, dylan.watson@broncbotz.org
\*********************************************************************/

#include "Drive.h"
#include "LoopChecks.h"
#include <iostream>
#include <WPILib.h>

using namespace std;
using namespace Systems;

Drive::Drive(){}

void Drive::Initialize()
{
	mainLoop();
//	liftTalon = activeCollection->GetTalon("lift");
//	encTest = new frc::Encoder(0,1);
}

void Drive::AddControlDrive(ControlItem *control){
	driveControlCollection.push_back(control);
}

void Drive::AddControlOperate(ControlItem *control){
	operateControlCollection.push_back(control);
}

void Drive::mainLoop()
{
	while(_IsTeleoporated())
	{
		for(int i=0; i<(int)driveControlCollection.size();i++)
			(*driveControlCollection[i]).Update();

		for(int i=0; i<(int)operateControlCollection.size();i++)
			(*operateControlCollection[i]).Update();

//		cout << activeCollection->GetEncoder("enc0")->Get() << endl;

		Wait(0.05);
	}
}

