/*
 * SwitchRight.h
 *
 *  Created on: Mar 13, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_SWITCHRIGHT_H_
#define SRC_AUTONOMI_SWITCHRIGHT_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class SwitchRight
{
	private:
		ActiveCollection *activeCollection;
		string gameData;

	public:
		SwitchRight(ActiveCollection *_activeCollection);
		void Start();
		virtual ~SwitchRight(){};
};
}

#endif /* SRC_AUTONOMI_SWITCHRIGHT_H_ */
