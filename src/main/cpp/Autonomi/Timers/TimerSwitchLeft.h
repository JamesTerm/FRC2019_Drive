/*
 * LeftScoreCube.h
 *
 *  Created on: Mar 8, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_SWITCHLEFT_H_
#define SRC_AUTONOMI_SWITCHLEFT_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class TimerSwitchLeft
{
	private:
		ActiveCollection *activeCollection;
		double error = .0005;
		string gameData;

	public:
		TimerSwitchLeft(ActiveCollection *_activeCollection);
		void Start();
		virtual ~TimerSwitchLeft(){};
};
}

#endif /* SRC_AUTONOMI_SWITCHLEFT_H_ */
