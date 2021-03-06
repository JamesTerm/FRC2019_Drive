#pragma once

#include "FRC2019_Goals.h"

static bool SelectAuton(ActiveCollection *activeCollection, MultitaskGoal_ac *goal, string autonSelected, string positionSelected)
{
    bool isFound = true;
	if (autonSelected == "DEBUG")
	{
		goal->AddGoal(new Goal_VisionAlign(activeCollection, new VisionTarget(320, 20), 140.0)); //!120 sec timeout for DEBUG only
		return true;
	}
	else if (autonSelected == "NONE")
		return true;

    if(positionSelected == "Level 1 Left")
    {
        if(autonSelected == "DriveStraight")
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
        }
        else if(autonSelected == "OneHatchAuto")
        {
            //TODO
        }
        else
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
            isFound = false;
        }
    }
    else if(positionSelected == "Level 1 Center")
    {
        if(autonSelected == "DriveStraight")
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
        }
        else if(autonSelected == "OneHatchAuto")
        {
            //TODO
        }
        else
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
            isFound = false;
        }
    }
    else if(positionSelected == "Level 1 Right")
    {
        if(autonSelected == "DriveStraight")
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
        }
        else if(autonSelected == "OneHatchAuto")
        {
            //TODO
        }
        else
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
            isFound = false;
        }
    }
    else if(positionSelected == "Level 2 Left")
    {
        if(autonSelected == "DriveStraight")
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
        }
        else if(autonSelected == "OneHatchAuto")
        {
            //TODO
        }
        else
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
            isFound = false;
        }
    }
    else if(positionSelected == "Level 2 Right")
    {
        if(autonSelected == "DriveStraight")
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
        }
        else if(autonSelected == "OneHatchAuto")
        {
            //TODO
        }
        else
        {
            goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
            isFound = false;
        }
    }
    else
    {
        goal->AddGoal(new Goal_DriveStraight(activeCollection, new Feet(10.0),.75));
        isFound = false;
    }
    
    return isFound;
}
