//Chris's version of goals. just testing stuff.

#include "Goal.h"

////////////////Implemented Atomic Goals////////////////
/////GoalDoNothing/////

Goal::Goal_Status GoalDoNothing::Process(double deltaTime)
{
    //if(deltaTime >= timeOut) return eFailed; //this line will not be excluded in other goals

    if(m_status == eActive)
    {
        if(deltaTime>=m_timeOut) 
        {
            Terminate();
            return m_status;
        }
    }
    else
    {
        return eErrorNotActive;
    }
    return m_status;
}

void GoalDoNothing::Terminate()
{
    m_status = eCompleted; //doNothing has nothing to test. Other goals may test whether goal should complete or fail
}

/////GoalDriveWithTimer/////
Goal::Goal_Status GoalDriveWithTimer::Process(double deltaTime)
{
    if(m_status == eActive)
    {
        if(deltaTime >= m_timeOut)
        {
            Terminate();
            return m_status;
        }
        else
        {
            SetDrive(m_left, m_right, m_activeCollection);
        }
    }
}

void GoalDriveWithTimer::Terminate()
{
    StopDrive(m_activeCollection);
    m_status = eCompleted;
}