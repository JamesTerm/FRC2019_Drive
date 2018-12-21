/****************************** Header ******************************\
Class Name: ActiveCollection
File Name:	ActiveCollection.cpp
Summary: Stores all Components on the robot controlled by the software.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper
Email: cooper.ryan@centaurisoftware.co
\********************************************************************/

#include <string>
#include <iostream>
#include <utility>

#include <ActiveCollection.h>

#include "Components/VictorSPItem.h"
#include "Components/TalonSRXItem.h"
#include "Components/NavX.h"

using namespace std;
using namespace Components;
using namespace Configuration;

ActiveCollection::ActiveCollection(){}

template <class T>
T ActiveCollection::Get(string name){
	try{
		for(int i=0; i<(int)activeCollection.size();i++)
			if((*activeCollection[i]).name == (string)name)
				return &activeCollection[i];
		throw "AHHH";
	}
	catch(...){
		cout << "Cannot find component " << name << ", it does not exist in the active collection!\nCheck your type correlation!" << endl;
	}
}

VictorSPItem* ActiveCollection::GetVictor(string name){
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == (string)name){
				return (VictorSPItem*)activeCollection[i];
			}
		}
		throw "AHHH";
	}
	catch(...){
		cout << "Cannot find victor " << name << ", it does not exist in the active collection!\n" << endl;
	}
}

TalonSRXItem* ActiveCollection::GetTalon(string name){
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == (string)name){
				return (TalonSRXItem*)activeCollection[i];
			}
		}
		throw "AHHH";
	}
	catch(...){
		cout << "Cannot find talon " << name << ", it does not exist in the active collection!\n" << endl;
	}
}

DigitalInputItem* ActiveCollection::GetDI(string name){
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == (string)name){
				return (DigitalInputItem*)activeCollection[i];
			}
		}
		throw "AHHH";
	}
	catch(...){
		cout << "Cannot find DI " << name << ", it does not exist in the active collection!\n" << endl;
	}
}

EncoderItem* ActiveCollection::GetEncoder(string name){
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == (string)name){
				return (EncoderItem*)activeCollection[i];
			}
		}
		throw "AHHH";
	}
	catch(...){
		cout << "Cannot find encoder " << name << ", it does not exist in the active collection!\n" << endl;
	}
}

int ActiveCollection::GetSize(){
	return activeCollection.size();
}

NavX* ActiveCollection::GetNavX(){
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == "NavX"){
				return (NavX*)activeCollection[i];
			}
		}
		throw "AHHH";
	}
	catch(...){
		cout << "Cannot find the NavX, check the config." << endl;
	}
}

void ActiveCollection::Add(NativeComponent *component){
	try{
		activeCollection.push_back(component);
	}
	catch(...){
		cout << "Cannot add component " << (*component).name << endl;
	}
}

void ActiveCollection::AddInput(InputComponent *component){
	cout << "Adding " << component->GetName() << " to the input collection" << endl;
	try{
		inputCollection.push_back(component);
	}
	catch(...){
		cout << "Cannot add component " << (*component).name << " to the inputCollection!" << endl;
	}
	cout << "INPUT SIZE: " << inputCollection.size() << endl;
}

vector<NativeComponent*> ActiveCollection::GetRawComponent(){
	return activeCollection;
}
