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
        eFailed     //The goal has failed and will either re-plan or be removed on the next update
    };

    //child classes will impliment the following methods
    virtual Goal_Status Process() = 0;
    virtual void plan() = 0;
    virtual void Terminate() = 0;

    //constant standard functions
    Goal_Status GetStatus() const { return m_status; }

  private:
    Goal_Status m_status;
};

class AtomicGoal : public Goal
{
  public:
    //from Goal
    virtual Goal_Status Process() {}
    virtual void plan() {}
    virtual void Terminate() {}
};

class CompositeGoal : public Goal
{
  public:
    //from Goal
    virtual Goal_Status Process() {}
    virtual void plan() {}
    virtual void Terminate() {}

    virtual void AddSubgoal(Goal* goal) {m_subgoals.push(goal);}

    

  private:
    typedef stack<Goal*> SubgoalStack;
    SubgoalStack m_subgoals;
};