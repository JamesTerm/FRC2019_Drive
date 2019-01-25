#include <iostream>
#include "Goal.h"



class Goal_DoNothing : public AtomicGoal
{
    public:
    Goal_DoNothing(ActiveCollection* activeCollection, double timeOut)
    {
        m_Status = eInactive;
        m_timeOut = timeOut;
        m_activeCollection = activeCollection;
    }
    void Activate();
    virtual Goal::Goal_Status Process(double dTime_s);

    protected:
    ActiveCollection* m_activeCollection;
    double m_currentTime;
    double m_timeOut;
};

class Goal_DriveWithTimer : public Goal_DoNothing
{
    public:
    Goal_DriveWithTimer( ActiveCollection* activeCollection, double timeOut) : Goal_DoNothing(activeCollection, timeOut)
    {
        cout << "constructor " << m_timeOut << endl;
    }
    virtual Goal::Goal_Status Process(double dTime_s);
    void Terminate();
};
