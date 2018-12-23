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
#include "Singleton.h"

using namespace std;

//TODO: why is this a singleton??? Why did we do this...
//Can probably make this a static class with static constants
namespace Util{
class Constants : public Singleton<Constants>{
	friend class Singleton<Constants>;
public:
	Constants(){}
	virtual ~Constants(){}
	double EPSILON_MIN = std::numeric_limits<double>::epsilon();
	double MINIMUM_JOYSTICK_RETURN = 0.04;
};}

#endif /* SRC_UTIL_CONSTANTS_H_ */
