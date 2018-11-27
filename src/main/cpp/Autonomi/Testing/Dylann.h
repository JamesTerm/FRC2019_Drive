/*
 * Dylann.h
 *
 *  Created on: Mar 3, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_DYLANN_H_
#define SRC_AUTONOMI_DYLANN_H_

#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class Dylann
{
	private:
		ActiveCollection *activeCollection;

	public:
		Dylann(ActiveCollection *_activeCollection);
		void Start();
		virtual ~Dylann(){};
};
}

#endif
