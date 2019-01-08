/****************************** Header ******************************\
Class Name: InputComponent inherits NativeComponent
File Name:	InputComponent.h
Summary: Abstraction for all programmable robot input components.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_INPUTCOMPONENT_H_
#define SRC_COMPONENTS_INPUTCOMPONENT_H_

#include <string>

#include "NativeComponent.h"

using namespace std;

namespace Components
{
    class InputComponent : public NativeComponent
    {
        public:
	        virtual double Get() = 0;
	        virtual string GetName() = 0;
	        InputComponent() { }
	        InputComponent(string name) : NativeComponent(name) { }
	        virtual ~InputComponent() { }
    };
}

#endif /* SRC_COMPONENTS_INPUTCOMPONENT_H_ */
