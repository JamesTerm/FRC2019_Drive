/****************************** Header ******************************\
Class Name: OutputComponent inherits NativeComponent
File Name:	OutputComponent.h
Summary: Abstraction for all programmable robot output components.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_OUTPUTCOMPONENT_H_
#define SRC_COMPONENTS_OUTPUTCOMPONENT_H_

#include "NativeComponent.h"

using namespace std;

namespace Components
{
	class OutputComponent : public NativeComponent
	{
		public:
			bool inUse = false;
			virtual void Set(double val) = 0;
			virtual double Get() = 0;
			OutputComponent(){}
			OutputComponent(string name) : NativeComponent(name){}

			virtual void DefaultSet() = 0;
			virtual void Set(DoubleSolenoid::Value value) = 0;
			virtual ~OutputComponent(){}
	};
}

#endif /* SRC_COMPONENTS_OUTPUTCOMPONENT_H_ */
