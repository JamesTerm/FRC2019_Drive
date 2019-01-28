/****************************** Header ******************************\
Class Name: VictorSPItem inherits OutputComponent
File Name:	VictorSPItem.h
Summary: Abstraction for the WPIlib VictorSP that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_VICTORSPITEM_H_
#define SRC_COMPONENTS_VICTORSPITEM_H_

#include "OutputComponent.h"

namespace Components
{
	class VictorSPItem final : public OutputComponent
	{
		public:
			VictorSPItem(){}
			virtual ~VictorSPItem(){}
			VictorSPItem(string name, int channel, bool reversed) : OutputComponent(name)
			{
				victor = new VictorSP(channel);
				Reversed = reversed;
				PDBChannel = 20;
			}

			VictorSPItem(string name, int channel, bool reversed, int _PDBChannel) : OutputComponent(name)
			{
					victor = new VictorSP(channel);
					Reversed = reversed;
					PDBChannel = _PDBChannel;
			}

			int GetPolarity();
			string GetName();
			int PDBChannel;
			void Stop();
			virtual void Set(double val) override;
			virtual double Get() override;
			//for reference
			//operator VictorSP *() { return victor;}
			VictorSP *AsVictorSP() { return victor; }
		private:
			VictorSP *victor;
			bool Reversed;
			virtual void DefaultSet() override;
			virtual void Set(DoubleSolenoid::Value value) override;
	};
}

#endif /* SRC_COMPONENTS_VICTORSPITEM_H_ */
