#pragma once

#include "FRC2019_Goals.h"

static bool SelectAuton(ActiveCollection *activeCollection, MultitaskGoal *goal, string autonSelected)
{
    bool isFound = true;
    if (autonSelected.compare("DriveStraight"))
        goal->AddGoal(new Goal_DriveStraight(activeCollection, 100, .5));
    //TODO remaining selections
    else
    {
        goal->AddGoal(new Goal_DriveStraight(activeCollection, 100, .5));
        isFound = false;
    }

    return isFound;
}
