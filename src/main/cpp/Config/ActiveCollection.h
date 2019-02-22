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
#include "../Components/VictorSPXItem.h"
#include "../Components/TalonSRXItem.h"
#include "../Components/EncoderItem.h"
#include "../Components/PotentiometerItem.h"
#include "../Components/DoubleSolenoidItem.h"
#include "../Components/DigitalInputItem.h"
#include "../Components/NavX.h"
#include "../Base/EventMap.h"

using namespace std;
using namespace Components;

namespace Configuration
{
	class ActiveCollection
	{
		public:
			enum ComponentType
			{
				VictorSP
			};

			ActiveCollection();
			virtual ~ActiveCollection(){}

			NativeComponent* Get(string name);
			VictorSPItem* GetVictor(string name);
			TalonSRXItem* GetTalon(string name);
			EncoderItem* GetEncoder(string name);
			NavX* GetNavX();
			int GetSize();
			vector<NativeComponent*> GetRawComponent();
			void Add(NativeComponent *component);
			Framework::Base::EventMap &GetEventMap() { return *m_EventMap; }
			//You can't share the pointer unless you offer it shared
			std::shared_ptr<Framework::Base::EventMap> &GetEventMap_Shared() { return m_EventMap; }
	private:
		vector<NativeComponent*> activeCollection;
		std::shared_ptr<Framework::Base::EventMap> m_EventMap;  //This makes it possble to fire events to anywhere that accesses this object
	};
}

#endif /* SRC_CONFIG_ACTIVECOLLECTION_H_ */
