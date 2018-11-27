/*
 * Dylann.h
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_TestNavX_H_
#define SRC_AUTONOMI_TestNavX_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class TestNavX
{
	private:
		ActiveCollection *activeCollection;

	public:
		TestNavX(ActiveCollection *_activeCollection);
		void Start();
		virtual ~TestNavX(){};
};
}

#endif
