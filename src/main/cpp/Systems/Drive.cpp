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
using namespace System;

Drive::Drive() { }

void Drive::AddControlDrive(ControlItem *control)
{
	driveControlCollection.push_back(control);
}

void Drive::AddControlOperate(ControlItem *control)
{
	operateControlCollection.push_back(control);
}

void Drive::Update()
{
	for(int i=0; i<(int)driveControlCollection.size();i++)
		(*driveControlCollection[i]).Update();

	for(int i=0; i<(int)operateControlCollection.size();i++)
		(*operateControlCollection[i]).Update();
}