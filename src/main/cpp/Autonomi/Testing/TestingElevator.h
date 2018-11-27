/*
 * Dylann.h
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_TESTINGELEVATOR_H_
#define SRC_AUTONOMI_TESTINGELEVATOR_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class TestingElevator
{
	private:
		ActiveCollection *activeCollection;

	public:
		TestingElevator(ActiveCollection *_activeCollection);
		void Start();
		virtual ~TestingElevator(){};
};
}

#endif
