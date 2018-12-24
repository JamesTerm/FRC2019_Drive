/****************************** Header ******************************\
Class Name:	Drive
File Name:	Drive.cpp
Summary: 	Drive loop
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ryan Cooper, Dylan Watson
Email:	cooper.ryan@centaurisoftware.co, dylan.watson@broncbotz.org
\*********************************************************************/

#include "Drive.h"

using namespace std;
using namespace Systems;

Drive::Drive(){}

void Drive::Initialize()
{
	mainLoop();
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

		Wait(0.05);
	}
}