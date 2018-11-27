/*
 * ScoreCube.h
 *
 *  Created on: Mar 4, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_SWITCHCENTER_H_
#define SRC_AUTONOMI_SWITCHCENTER_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class TimerSwitchCenter
{
	private:
		ActiveCollection *activeCollection;
		double error = .01;
		bool left = true;
		string gameData;

	public:
		TimerSwitchCenter(ActiveCollection *_activeCollection);
		void Start();
		virtual ~TimerSwitchCenter(){};
};
}

#endif /* SRC_AUTONOMI_SWITCHCENTER_H_ */
