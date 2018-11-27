/*
 * ChampsTwoCube.h
 *
 *  Created on: Apr 19, 2018
 *      Author: Dylann Ruiz
 */

#ifndef SRC_AUTONOMI_CHAMPS_CHAMPSTWOCUBE_H_
#define SRC_AUTONOMI_CHAMPS_CHAMPSTWOCUBE_H_


#include "Config/ActiveCollection.h"
#include <string>

using namespace std;
using namespace Configuration;

namespace Autonomi{
class ChampsTwoCube
{
	private:
		ActiveCollection *activeCollection;
		string gameData;

	public:
		ChampsTwoCube(ActiveCollection *_activeCollection);
		void Start();
		virtual ~ChampsTwoCube(){};
};
}

#endif /* SRC_AUTONOMI_CHAMPS_CHAMPSTWOCUBE_H_ */
