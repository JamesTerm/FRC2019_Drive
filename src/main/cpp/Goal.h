//Chris's version of goals. just testing stuff.
#include <stack>

using namespace std;

class Goal
{
  public:
    enum Goal_Status
    {
        eInactive,  //The goal is waiting to be activated
        eActive,    //The goal has been activated and will be processed each update step
        eCompleted, //The goal has completed and will be removed on the next update
        eFailed,     //The goal has failed and will either re-plan or be removed on the next update
        eErrorNotActive
    };
    virtual void activate() {m_status = eActive;} //set status to active

    //child classes will impliment the following methods
    virtual Goal_Status Process() = 0; //execute goals
    virtual void Plan() = 0; //decide what to do next
    virtual void Terminate() = 0; //end the goal

    //constant standard functions
    

  protected:
    Goal_Status m_status;
};

class AtomicGoal : public Goal
{
  public:
    //from Goal
    virtual Goal_Status Process() {}
    virtual void Plan() {}
    virtual void Terminate() {}

};

class CompositeGoal : public Goal
{
  public:
    //from Goal
    virtual Goal_Status Process() {}
    virtual void Plan() {}
    virtual void Terminate() {}

    virtual void AddSubgoal(Goal* goal) {m_subgoals.push(goal);}
    
    
  protected:
    typedef stack<Goal*> SubgoalStack;
    SubgoalStack m_subgoals;
};

////////////////Implemented Atomic Goals////////////////


//the purpose of this goal is to be the template for other implemented goals.
//If instatiated, it, obviously, wont do anything
class GoalDoNothing : public AtomicGoal
{
  public:
    GoalDoNothing(int milliseconds) {m_status = eInactive; time = milliseconds;}
    void Plan();
    Goal_Status Process();
    void Terminate();
  protected:
    int time;
};
