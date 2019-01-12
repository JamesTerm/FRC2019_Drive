#include "TemplateTest.h"

using namespace std;
using namespace Autonomi;
using namespace Util;
using namespace Configuration;

TemplateTest::TemplateTest(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void TemplateTest::Start()
{
	cout << "template test" << endl;
	VictorSPItem *victor = (VictorSPItem*)(activeCollection->Get("left_0"));
//	VictorSPItem *vic = &victor;
}