/*
 * ScaleLeft.h
 *
 *  Created on: Mar 27, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_SENSORS_SCALE_SCALELEFT_H_
#define SRC_AUTONOMI_SENSORS_SCALE_SCALELEFT_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class ScaleLeft
{
	private:
		ActiveCollection *activeCollection;
		string gameData;
	public:
		ScaleLeft(ActiveCollection *_activeCollection);
		void Start();
		virtual ~ScaleLeft(){};
};
}

#endif
