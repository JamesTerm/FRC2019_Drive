#pragma once

#include "FRC2019_Goals.h"

static bool SelectAuton(ActiveCollection *activeCollection, MultitaskGoal_ac *goal, string autonSelected)
{
    bool isFound = true;
    if (autonSelected.compare("DriveStraight") == 0)
    {
        goal->AddGoal(new Goal_DriveWithTimer(activeCollection, .5, .5, 5));
        cout << "drive straight" << endl;
    }
    else if (autonSelected.compare("DEBUG1") == 0)
    {
        goal->AddGoal(new Goal_Turn(activeCollection, 45));
        cout << "Debug 1" << endl;
    }

    //TODO remaining selections
    else
    {
        goal->AddGoal(new Goal_DriveStraight(activeCollection, 100, .5));
        isFound = false;
        cout << "Auton not found" << endl;
    }

    return isFound;
}
