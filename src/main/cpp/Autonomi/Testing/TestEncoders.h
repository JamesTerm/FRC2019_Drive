/*
 * Dylann.h
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_TESTENCODERS_H_
#define SRC_AUTONOMI_TESTENCODERS_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class TestEncoders
{
	private:
		ActiveCollection *activeCollection;

	public:
		TestEncoders(ActiveCollection *_activeCollection);
		void Start();
		virtual ~TestEncoders(){};
};
}

#endif
