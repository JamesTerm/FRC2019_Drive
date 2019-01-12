//Chris's version of goals. just testing stuff.
#include <stack>

#include "Util/UtilityFunctions.h"

using namespace std;

class Goal
{
public:
  enum Goal_Status
  {
    eInactive,  //The goal is waiting to be activated
    eActive,    //The goal has been activated and will be processed each update step
    eCompleted, //The goal has completed and will be removed on the next update
    eFailed,    //The goal has failed and will either re-plan or be removed on the next update
    eErrorNotActive
  };
  virtual void activate() { m_status = eActive; } //set status to active

  //child classes will impliment the following methods
  virtual Goal_Status Process(double deltaTime) = 0; //execute goals
  virtual void Terminate() = 0;                      //end the goal and set status

  //constant standard functions

protected:
  Goal_Status m_status;
};

class AtomicGoal : public Goal
{
public:
  //from Goal
  virtual Goal_Status Process(double deltaTime) {}
  virtual void Terminate() {}
};

class CompositeGoal : public Goal
{
public:
  //from Goal
  virtual Goal_Status Process(double deltaTime) {}
  virtual void Terminate() {}

  virtual void AddSubgoal(Goal *goal) { m_subgoals.push(goal); }

protected:
  typedef stack<Goal *> SubgoalStack;
  SubgoalStack m_subgoals;
};

////////////////Implemented Atomic Goals////////////////

//the purpose of this goal is to be the template for other implemented goals.
//If instatiated, it, obviously, wont do anything
class GoalDoNothing : public AtomicGoal
{
public:
  GoalDoNothing(double seconds)
  {
    m_status = eInactive;
    m_timeOut = seconds;
  }
  Goal_Status Process(double deltaTime);
  void Terminate();

protected:
  double m_timeOut;
};

class GoalDriveWithTimer : public GoalDoNothing
{
public:
  GoalDriveWithTimer(double seconds, double leftPower, double rightPower, ActiveCollection* activeCollection) : GoalDoNothing(seconds) 
  {
    m_left = leftPower;
    m_right = rightPower;
  }
  Goal_Status Process(double deltaTime);
  void Terminate();
protected:
  double m_left, m_right;
  ActiveCollection* m_activeCollection;
};

class GoalWaitThenDrive : public CompositeGoal
{
  
};