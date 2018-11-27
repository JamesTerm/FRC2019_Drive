/****************************** Header ******************************\
Class Name: NativeComponent
File Name:	OutputComponent.h
Summary: Abstraction for all programmable robot components.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylan.watson@broncbotz.org
\********************************************************************/

#ifndef SRC_COMPONENTS_NATIVECOMPONENT_H_
#define SRC_COMPONENTS_NATIVECOMPONENT_H_

#include <string>

using namespace std;

namespace Components{
class NativeComponent
{

public:
	string name;
	NativeComponent(){}
	NativeComponent(string _name){
		name = _name;
	}
	virtual ~NativeComponent(){}
};
}

#endif /* SRC_COMPONENTS_NATIVECOMPONENT_H_ */
