/*
 * RealNibbaShit.h
 *
 *  Created on: Apr 2, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_TESTING_REALNIBBASHIT_H_
#define SRC_AUTONOMI_TESTING_REALNIBBASHIT_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class RealNibbaShit
{
	private:
		ActiveCollection *activeCollection;

	public:
		RealNibbaShit(ActiveCollection *_activeCollection);
		void Start();
		virtual ~RealNibbaShit(){};
};
}

#endif /* SRC_AUTONOMI_TESTING_REALNIBBASHIT_H_ */

