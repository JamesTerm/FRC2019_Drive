/****************************** Header ******************************\
Class Name: DriveStraight
File Name:	Drivestraight.h
Summary:	An autonomous program to make the robot drive straight
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylann Ruiz
Email: ruizdylann@gmail.com
\********************************************************************/
#ifndef SRC_AUTONOMI_TEMPLATETEST_H_
#define SRC_AUTONOMI_TEMPLATETEST_H_

#include <string>
#include <frc/WPILib.h>
#include <fstream>
#include <thread>
#include <iostream>

#include "../Util/UtilityFunctions.h"
#include "../Config/ActiveCollection.h"
#include "../Config/Config.h"
#include "../Components/VictorSPItem.h"

using namespace std;
using namespace Configuration;

namespace Autonomi{
class TemplateTest
{
	private:
		ActiveCollection *activeCollection;

	public:
		TemplateTest(ActiveCollection *_activeCollection);
		void Start();
		virtual ~TemplateTest(){};
};
}

#endif /* SRC_AUTONOMI_TEMPLATETEST_H_ */