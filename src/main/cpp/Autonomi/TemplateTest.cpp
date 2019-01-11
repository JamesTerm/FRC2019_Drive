#include "TemplateTest.h"

using namespace std;
using namespace Autonomi;
using namespace Util;

TemplateTest::TemplateTest(ActiveCollection *_activeCollection)
{
	activeCollection = _activeCollection;
}

void TemplateTest::Start()
{
	cout << "template test" << endl;
	activeCollection->Get<VictorSPItem*>("left_0");
}