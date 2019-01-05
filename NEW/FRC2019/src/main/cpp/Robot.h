/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/WPILib.h>

#include <string>
#include <frc/SampleRobot.h>
#include "ActiveCollection.h"
#include "Systems/Drive.h"
#include "Controls/AxisControl.h"

using namespace frc;
using namespace System;
using namespace Controls;

#define VERSION 1 //!< Defines the program version for the entire program.
#define REVISION "A" //!< Defines the revision of this version of the program.


class Robot : public SampleRobot 
{   
    public:
        Robot();

        void RobotInit() override;
        void Autonomous() override;
        void OperatorControl() override;
        void Test() override;

    private:
	    Drive *m_drive;
	    ActiveCollection *m_activeCollection; //!< Pointer to the only instantiation of the ActiveCollection Class in the program
};
