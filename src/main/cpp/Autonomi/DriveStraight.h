/*
 * Dylann.h
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_DRIVESTRAIGHT_H_
#define SRC_AUTONOMI_DRIVESTRAIGHT_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class DriveStraight
{
	private:
		ActiveCollection *activeCollection;
		double error = .01;
		string gameData;

	public:
		DriveStraight(ActiveCollection *_activeCollection);
		void Start();
		virtual ~DriveStraight(){};
};
}

#endif /* SRC_AUTONOMI_DRIVESTRAIGHT_H_ */
