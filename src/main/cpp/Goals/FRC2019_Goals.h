#include <iostream>
#include "Goal.h"



class Goal_DoNothing : public AtomicGoal
{
    public:
    Goal_DoNothing(double timeOut) : m_timeOut(timeOut)
    {
        m_Status = eInactive;
    }
    void Activate();
    Goal::Goal_Status Process(double dTime_s);
    private:
    double m_currentTime;
    double m_timeOut;
};