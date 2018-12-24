/****************************** Header ******************************\
Class Name: Constants inherits Singleton
File Name: Constants.h
Summary: File of constant values to be used throughout the program.
Project: FRC2019CPP
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watsonr
Email: cooper.ryan@centaurisoftware.co, dylan.watson@broncbotz.org
\********************************************************************/

#ifndef SRC_UTIL_CONSTANTS_H_
#define SRC_UTIL_CONSTANTS_H_

#include <cmath>
#include <limits>

using namespace std;

//TODO: why is this a singleton??? Why did we do this...
//Can probably make this a static class with static constants
namespace Util
{
	static const double MINIMUM_JOYSTICK_RETURN = 0.04;
	static const double EPSILON_MIN = std::numeric_limits<double>::epsilon();
}

#endif /* SRC_UTIL_CONSTANTS_H_ */
