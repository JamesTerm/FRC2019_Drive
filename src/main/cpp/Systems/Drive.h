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

#include "Preproc.h"
#include "../Util/LoopChecks.h"
#include "../Controls/ControlItem.h"
#include "../Config/ActiveCollection.h"

using namespace std;
using namespace Configuration;
using namespace Controls;

namespace Systems{
class Drive final
{
public:
		bool enabled = true;

	    Drive();
	    void Update(double dTime_s);
	    void AddControlDrive(ControlItem *control);
	    void AddControlOperate(ControlItem *control);
	    vector<ControlItem*> driveControlCollection;
	    vector<ControlItem*> operateControlCollection;
		void Initialize() __attribute__((deprecated(UNBOUNDED)));

private:
		Joystick *m_driver;
		Joystick *m_operateJoy;
};

} /* namespace Systems */

#endif /* SYSTEMS_DRIVE_H_ */
