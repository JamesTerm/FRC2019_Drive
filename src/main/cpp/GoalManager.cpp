/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "GoalManager.h"

using namespace std;

GoalManager::GoalManager() 
{
    
}

void GoalManager::addGoal(Goal* goal)
{
    m_goalStack.push(goal);
}

void execute()
{
    m_goalStack.pop();
}