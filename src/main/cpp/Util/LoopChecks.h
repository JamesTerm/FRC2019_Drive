/****************************** Header ******************************\
Class Name: -
File Name: LoopChecks.h
Summary: Utility file to check for loop security.
Project: FRC2019CPP
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper
Email: cooper.ryan@centaurisoftware.co
\********************************************************************/

#ifndef SRC_INCLUDE_LOOPCHECKS_H_
#define SRC_INCLUDE_LOOPCHECKS_H_

#include <frc/WPILib.h>

using namespace frc;

namespace Util
{
	inline static bool _IsAutononomous()
	{
		return DriverStation::GetInstance().IsAutonomous() && DriverStation::GetInstance().IsEnabled();
	}

	inline static bool _IsTeleoporated()
	{
		return DriverStation::GetInstance().IsOperatorControl() && DriverStation::GetInstance().IsEnabled();
	}
}


#endif /* SRC_INCLUDE_LOOPCHECKS_H_ */
