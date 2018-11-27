/*
 * SwitchCenter.h
 *
 *  Created on: Apr 30, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_SENSORS_SWITCH_SWITCHCENTER_H_
#define SRC_AUTONOMI_SENSORS_SWITCH_SWITCHCENTER_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class SwitchCenter
{
	private:
		ActiveCollection *activeCollection;
		string gameData;

	public:
		SwitchCenter(ActiveCollection *_activeCollection);
		void Start();
		virtual ~SwitchCenter(){};
};
}

#endif /* SRC_AUTONOMI_SENSORS_SWITCH_SWITCHCENTER_H_ */
