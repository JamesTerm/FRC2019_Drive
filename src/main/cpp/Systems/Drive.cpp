/****************************** Header ******************************\
Class Name:	Drive
File Name:	Drive.cpp
Summary: 	Drive loop
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ryan Cooper, Dylan Watson
Email:	cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com
\*********************************************************************/

#include "Drive.h"
#include <iostream>

using namespace std;
using namespace Systems;

Drive::Drive(){
	cout << "NEW DRIVE" << endl;
}

void Drive::Initialize()
{
	cout << "INITIALIZE DRIVE" << endl;
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