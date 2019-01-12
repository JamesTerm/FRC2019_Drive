//Chris's version of goals. just testing stuff.

#include "Goal.h"

////////////////Implemented Atomic Goals////////////////
/////GoalDoNothing/////
void GoalDoNothing::Plan()
{
    //TODO: decide what to check when planning
}

Goal::Goal_Status GoalDoNothing::Process()
{
    if(m_status == eActive)
    {
        //this goal does not do anything
        return eCompleted;
    }
    else
    {
        return eErrorNotActive;
    }
    return eFailed; //if somehow it gets past your code, fail
}

void GoalDoNothing::Terminate()
{
}