/****************************** Header ******************************\
Class Name: ActiveCollection
File Name:	ActiveCollection.h
Summary: Stores all Components on the robot controlled by the software.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper
Email: cooper.ryan@centaurisoftware.co
\********************************************************************/

#ifndef SRC_CONFIG_ACTIVECOLLECTION_H_
#define SRC_CONFIG_ACTIVECOLLECTION_H_

#include <vector>
#include <utility>

#include "../Components/VictorSPItem.h"
#include "../Components/TalonSRXItem.h"
#include "../Components/DigitalInputItem.h"
#include "../Components/EncoderItem.h"
#include "../Components/NavX.h"
#include "../Components/DoubleSolenoidItem.h"

using namespace std;
using namespace Components;

namespace Configuration{

class ActiveCollection{
public:

	enum ComponentType{
			VictorSP
	};
	vector<InputComponent*> inputCollection;

	ActiveCollection();
	virtual ~ActiveCollection(){}

	//TODO: Look into making the add methods bools
	template <class T>
	T Get(string name);
	VictorSPItem* GetVictor(string name);
	TalonSRXItem* GetTalon(string name);
	DigitalInputItem* GetDI(string name);
	EncoderItem* GetEncoder(string name);
	NavX* GetNavX();
	int GetSize();
	vector<NativeComponent*> GetRawComponent();
	void Add(NativeComponent *component);
	void AddInput(InputComponent *component);


private:
	vector<NativeComponent*> activeCollection;
};

}
#endif /* SRC_CONFIG_ACTIVECOLLECTION_H_ */
