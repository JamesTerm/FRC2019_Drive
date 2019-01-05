/****************************** Header ******************************\
Class Name:	Drive [final]
File Name: 	Drive.h
Summary: 	Drive loop
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ryan Cooper, Dylan Watson
Email:	cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com
\*********************************************************************/
#ifndef SYSTEMS_DRIVE_H_
#define SYSTEMS_DRIVE_H_

#include <frc/WPILib.h>
#include "../util/LoopChecks.h"
#include "../Controls/ControlItem.h"

using namespace frc;
using namespace std;
using namespace Util;
using namespace Controls;

namespace System
{
    class Drive final
    {
        public:
	        Drive();
	        void Update();
	        void AddControlDrive(ControlItem *control);
	        void AddControlOperate(ControlItem *control);
	        vector<ControlItem*> driveControlCollection;
	        vector<ControlItem*> operateControlCollection;
    };
} /* namespace Systems */

#endif /* SYSTEMS_DRIVE_H_ */
