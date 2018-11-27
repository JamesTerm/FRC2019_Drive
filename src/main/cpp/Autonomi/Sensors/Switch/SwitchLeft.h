/*
 * SwitchLeft.h
 *
 *  Created on: Mar 13, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_SWITCHLEFT_H_
#define SRC_AUTONOMI_SWITCHLEFT_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class SwitchLeft
{
	private:
		ActiveCollection *activeCollection;
		string gameData;

	public:
		SwitchLeft(ActiveCollection *_activeCollection);
		void Start();
		virtual ~SwitchLeft(){};
};
}

#endif /* SRC_AUTONOMI_SWITCHLEFT_H_ */
