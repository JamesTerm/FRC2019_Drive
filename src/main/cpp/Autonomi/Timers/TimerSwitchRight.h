/*
 * RightScoreCube.h
 *
 *  Created on: Mar 8, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_SWITCHRIGHT_H_
#define SRC_AUTONOMI_SWITCHRIGHT_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class TimerSwitchRight
{
	private:
		ActiveCollection *activeCollection;
		double error = .0005;
		string gameData;

	public:
		TimerSwitchRight(ActiveCollection *_activeCollection);
		void Start();
		virtual ~TimerSwitchRight(){};
};
}

#endif /* SRC_AUTONOMI_SWITCHRIGHT_H_ */
